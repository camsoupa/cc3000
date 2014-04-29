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

led_state * add_led_state(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness, uint32_t pulse_ms, uint32_t duration_ms)
{
	led_state * new_state = (led_state *)malloc(sizeof(led_state));
	new_state->r = r;
	new_state->g = g;
	new_state->b = b;
	new_state->brightness = brightness;
	new_state->mode = TRANS_ON_MIN;
	new_state->pulse_rate_ms = pulse_ms;
	new_state->duration_ms = duration_ms;
	new_state->next = 0;

	insert_led_state(new_state, 0);
	return new_state;
}

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

void led_state_values_changed() {
	if(head) {
		set_led_state(head);
	}
}

void start_led_state_timer(led_state * ls) {
	timer_disable(led_state_duration_timer_id);
	timer_setOverflowVal(led_state_duration_timer_id, head->duration_ms*HZ_PER_MS);
	timer_enable_allInterrupts(led_state_duration_timer_id);
	timer_enable_overflowInt(led_state_duration_timer_id);
	timer_enable(led_state_duration_timer_id);
}


void init_rgb_pwm(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b, uint8_t _gpio_i){
	timer_init();

	MSS_GPIO_config((gpio_r_i = _gpio_r), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config((gpio_g_i = _gpio_g), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config((gpio_b_i = _gpio_b), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config((gpio_i_i = _gpio_i), MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config(LED_STATE_GPIO_INT, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);

	MSS_GPIO_enable_irq(_gpio_r);
	MSS_GPIO_enable_irq(_gpio_g);
	MSS_GPIO_enable_irq(_gpio_b);
	MSS_GPIO_enable_irq(_gpio_i);
	//MSS_GPIO_enable_irq(LED_STATE_GPIO_INT);

	// add the timer peripherals to the timer module
	red_timer_id   = add_timer(TIMER_RED);
	green_timer_id = add_timer(TIMER_GREEN);
	blue_timer_id  = add_timer(TIMER_BLUE);
	pulse_timer_id = add_timer(TIMER_PULSE);
	//led_state_duration_timer_id = add_timer(TIMER_LED_STATE_DURATION);

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
	//timer_enable(pulse_timer_id);

	// don't start the colors until set_brightness is called
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
			//uint8_t mode = head->mode;
			//uint32_t trans_on_max = (head->mode & TRANS_ON_MAX);
			//if(trans_on_max) {
			//	transition_to_next_state();
			//}
		}
	} else {
		if(master_brightness > min_brightness) {
			set_brightness(master_brightness-1);
		} else {
			pulse_direction = BRIGHTER;

			timer_setOverflowVal(red_timer_id, 1000000);
			timer_setOverflowVal(blue_timer_id, 1000000);
			timer_setOverflowVal(green_timer_id, 1000000);
			head->b = head->next->b;
			head->r = head->next->r;
			head->g = head->next->g;
			head->pulse_rate_ms = head->next->pulse_rate_ms;
			set_led_state(head);
			//timer_enable(red_timer_id);
			//timer_enable(blue_timer_id);
			//timer_enable(green_timer_id);
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
    //if(!head->pulse_ms && duration_reached)
    //    transition_to_next_state();
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
// param: rate - the number of cycles to wait before interrupting (which changes master_brightness)
// (0 = no pulse, 1=fast pulse 1-2^32=slower pulse)
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

__attribute__ ((interrupt)) void GPIO14_IRQHandler(void){
	//on_led_state_duration_reached();
}

/*
void transition_to_next_state() {
	if(duration_reached) {

		//clear the flag
		duration_reached = 0;

		// if there are no queued states or it is the last state do nothing
		// the last state will continue until a new state is queued
		if(!head) return;

		if(head->next != 0) {
			head = head->next;
		}

		set_led_state(head);
		show_color(red);
		show_color(green);
		show_color(blue);
		start_led_state_timer(head);
	}
}*/

//void on_led_state_duration_reached() {
//	// calling this clears the interrupt
//	MSS_GPIO_clear_irq(LED_STATE_GPIO_INT);
//	timer_getInterrupt_status(led_state_duration_timer_id);
//	duration_reached = 1;
//}



