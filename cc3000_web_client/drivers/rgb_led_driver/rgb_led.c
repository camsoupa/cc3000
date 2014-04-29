/*
 * rgb_led.c
 *
 * Version 2
 */

#include <stdio.h>
#include <stdlib.h>
#include "./rgb_led.h"
#include "../mss_gpio/mss_gpio.h"
#include "../fpga_timer/fpga_timer.h"

#define show_color(color) \
	if(color) \
		timer_enable(color ## _timer_id);

//TODO: standardize naming scheme

#define update_color(old, new, enable) \
	if(!new) { \
		timer_disable(old ## _timer_id); \
		MSS_GPIO_set_output(gpio_ ## new, OFF); \
	} else { \
		if(!old && enable) \
			timer_enable(old ## _timer_id); \
	}


// pulse direction
#define DIMMER  0
#define BRIGHTER  1

#define ON 1
#define OFF 0

#define HZ_PER_COLOR_DIVISION (1000000/255)

// timer status register reason for interrupt
#define STATUS_OVERFLOW 0x01
#define STATUS_COMPARE  0x02

// Timer Peripherals
// each peripheral is allotted 32 bytes, thus the offset
#define TIMER_RED   ((mytimer_t *)0x40050100)
#define TIMER_GREEN ((mytimer_t *)0x40050200)
#define TIMER_BLUE  ((mytimer_t *)0x40050300)
#define TIMER_PULSE ((mytimer_t *)0x40050400)
#define TIMER_LED_STATE_DURATION ((mytimer_t *)0x40050500)

extern void* g_pfnVectors;

// the id of the timer returned by add_timer()
// allows access to compare value, status, overflow, etc.
uint8_t red_timer_id;
uint8_t green_timer_id;
uint8_t blue_timer_id;
uint8_t pulse_timer_id;
uint8_t led_state_duration_timer_id;

uint8_t duration_reached = 1;


//0-100
float master_brightness;
float full_brightness = 100;
float min_brightness = 1;

uint8_t pulse_direction;

//the rgb color of the led
uint8_t red;
uint8_t green;
uint8_t blue;

//display
uint8_t gpio_r;
uint8_t gpio_g;
uint8_t gpio_b;
uint8_t gpio_i;

//interrupts
uint8_t gpio_r_i;
uint8_t gpio_g_i;
uint8_t gpio_b_i;
uint8_t gpio_i_i;

led_state * head;

void set_led_state(led_state * ls) {
	set_color(ls->r,ls->g,ls->b); set_brightness(ls->brightness); set_pulse_rate(ls->pulse_rate_ms);
}

void insert_led_state(led_state * state, led_state * after) {
	if(after) {
		state->next = after->next;
		after->next = state;
	} else if(head){
		// insert at end
		led_state * curr = head;
		led_state * prev = 0;

		do {
			prev = curr;
			curr = curr->next;
		} while(curr != 0 && curr != head);

		state->next = prev->next;
		prev->next = state;
	} else {
		// the first one
		head = state;
	}
}

void start_led_sequence() {
	if(head) {
		set_led_state(head);
		timer_enable(red_timer_id);
		timer_enable(blue_timer_id);
		timer_enable(green_timer_id);
	}
}

void init_rgb_pwm(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b, uint8_t _gpio_i){
	timer_init();

	MSS_GPIO_config((gpio_r_i = _gpio_r), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config((gpio_g_i = _gpio_g), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config((gpio_b_i = _gpio_b), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config((gpio_i_i = _gpio_i), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);

	MSS_GPIO_enable_irq(_gpio_r);
	MSS_GPIO_enable_irq(_gpio_g);
	MSS_GPIO_enable_irq(_gpio_b);
	MSS_GPIO_enable_irq(_gpio_i);

	// add the timer peripherals to the timer module
	red_timer_id   = add_timer(TIMER_RED);
	green_timer_id = add_timer(TIMER_GREEN);
	blue_timer_id  = add_timer(TIMER_BLUE);
	pulse_timer_id = add_timer(TIMER_PULSE);

	// all colors overflow at their max pwm value
	timer_setOverflowVal(red_timer_id, 1000000);
	timer_setOverflowVal(blue_timer_id, 1000000);
	timer_setOverflowVal(green_timer_id, 1000000);

	// pulse uses overflow only
	//timer_enable_allInterrupts(pulse_timer_id);
	timer_enable_overflowInt(pulse_timer_id);
	timer_disable_compareInt(pulse_timer_id);

	// colors require overflow and compare registers
	timer_enable_allInterrupts(red_timer_id);
	timer_enable_allInterrupts(blue_timer_id);
	timer_enable_allInterrupts(green_timer_id);

	timer_enable_compareInt(red_timer_id);
	timer_enable_compareInt(blue_timer_id);
	timer_enable_compareInt(green_timer_id);

	timer_enable_overflowInt(red_timer_id);
	timer_enable_overflowInt(blue_timer_id);
	timer_enable_overflowInt(green_timer_id);
}

// caller must have called MSS_GPIO_init();
// because we don't want to re-init and change other gpio's setup by caller
void init_rgb_led(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b){
	MSS_GPIO_config((gpio_r = _gpio_r), MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_config((gpio_g = _gpio_g), MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_config((gpio_b = _gpio_b), MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_set_output(gpio_r, OFF);
	MSS_GPIO_set_output(gpio_g, OFF);
	MSS_GPIO_set_output(gpio_b, OFF);
}

void on_pulse(void){
	MSS_GPIO_clear_irq(gpio_i_i);
	if(pulse_direction == BRIGHTER) {
		if(master_brightness < full_brightness) {
			set_brightness(master_brightness+1);
		} else {
			pulse_direction = DIMMER;
		}
	} else {
		if(master_brightness > min_brightness) {
			set_brightness(master_brightness-1);
		} else {
			pulse_direction = BRIGHTER;

			timer_setOverflowVal(red_timer_id, 1000000);
			timer_setOverflowVal(blue_timer_id, 1000000);
			timer_setOverflowVal(green_timer_id, 1000000);

			// for now, we only update the led color state
			// when brightness reaches min
			// update the current values with the pending values
			head->b = head->next->b;
			head->r = head->next->r;
			head->g = head->next->g;
			head->pulse_rate_ms = head->next->pulse_rate_ms;
			set_led_state(head);
		}
	}
}

void pwm_red(){
	MSS_GPIO_clear_irq(gpio_r_i);
	pwm_timer_handler(gpio_r, red_timer_id);
}

void pwm_green(){
	MSS_GPIO_clear_irq(gpio_g_i);
	pwm_timer_handler(gpio_g, green_timer_id);
}

void pwm_blue(){
	MSS_GPIO_clear_irq(gpio_b_i);
	pwm_timer_handler(gpio_b, blue_timer_id);
}

void pwm_timer_handler(uint32_t gpio, uint32_t timer_index)
{
    uint32_t status = timer_getInterrupt_status(timer_index);

    if(status & STATUS_OVERFLOW)
    {
        MSS_GPIO_set_output(gpio, ON);
    }
    else if(status & STATUS_COMPARE)
    {
    	MSS_GPIO_set_output(gpio, OFF);
    }
}

void update_compare_values(){
	timer_setCompareVal(red_timer_id, (red*HZ_PER_COLOR_DIVISION)*master_brightness/full_brightness);
	timer_setCompareVal(blue_timer_id, (blue*HZ_PER_COLOR_DIVISION)*master_brightness/full_brightness);
	timer_setCompareVal(green_timer_id, (green*HZ_PER_COLOR_DIVISION)*master_brightness/full_brightness);
}


void set_color(uint8_t r, uint8_t g, uint8_t b){
	update_color(red, r, 1);
	update_color(green, g, 1);
	update_color(blue, b, 1);
	red = r; green = g; blue = b;
	update_compare_values();
}

void set_brightness(uint8_t brightness){
	master_brightness = (float)brightness;
	update_compare_values();
}


// pulse rate oscillates the led brightness between max_brightness and min_brightness
// ms: the desired duration from min to max brightness
void set_pulse_rate(uint32_t ms){
   timer_setOverflowVal(pulse_timer_id, ms*HZ_PER_MS);
   if(ms > 0) {
     timer_enable_allInterrupts(pulse_timer_id);
     timer_enable(pulse_timer_id);
   } else {
	 timer_disable_allInterrupts(pulse_timer_id);
	 timer_disable(pulse_timer_id);
   }
}

__attribute__ ((interrupt)) void GPIO5_IRQHandler(void){
	pwm_red();
}

__attribute__ ((interrupt)) void GPIO6_IRQHandler(void){
	pwm_green();
}

__attribute__ ((interrupt)) void GPIO7_IRQHandler(void){
	pwm_blue();
}

__attribute__ ((interrupt)) void GPIO8_IRQHandler(void){
	on_pulse();
}

