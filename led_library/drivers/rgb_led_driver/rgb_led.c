/*
 * rgb_led.c
 *
 */

#include "./rgb_led.h"
#include "../mss_gpio/mss_gpio.h"
#include "../fpga_timer/fpga_timer.h"

// pulse direction
#define DIMMER  0
#define BRIGHTER  1

// gpio led value (active low)
#define ON 0
#define OFF 1

// timer status register reason for interrupt
#define STATUS_OVERFLOW 0x01
#define STATUS_COMPARE  0x02

// Timer Peripherals
// each peripheral is allotted 32 bytes, thus the offset
#define TIMER_RED   ((mytimer_t *)0x40050100)
#define TIMER_GREEN ((mytimer_t *)0x40050200)
#define TIMER_BLUE  ((mytimer_t *)0x40050300)
#define TIMER_PULSE ((mytimer_t *)0x40050400)

// the id of the timer returned by add_timer()
// allows access to compare value, status, overflow, etc.
uint8_t red_timer_id;
uint8_t green_timer_id;
uint8_t blue_timer_id;
uint8_t pulse_timer_id;

//0-100
uint8_t master_brightness = 100;

uint8_t full_brightness = 100;
uint8_t min_brightness = 0;
uint8_t pulse_direction;

//the rgb color of the led
uint8_t red;
uint8_t green;
uint8_t blue;

uint8_t gpio_r;
uint8_t gpio_g;
uint8_t gpio_b;
uint8_t gpio_i;

void init_rgb_pwm(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b, uint8_t _gpio_i){
	MSS_GPIO_config(_gpio_r, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE);
	MSS_GPIO_config(_gpio_g, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE);
	MSS_GPIO_config(_gpio_b, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE);
	MSS_GPIO_config(_gpio_i, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE);

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
	timer_setOverflowVal(red_timer_id, 255);
	timer_setOverflowVal(blue_timer_id, 255);
	timer_setOverflowVal(green_timer_id, 255);
	timer_setOverflowVal(pulse_timer_id, 0);

	// pulse uses overflow only
	timer_enable_overflowInt(pulse_timer_id);
	timer_disable_compareInt(pulse_timer_id);

	// colors require overflow and compare registers
	timer_enable_allInterrupts(red_timer_id);
	timer_enable_allInterrupts(blue_timer_id);
	timer_enable_allInterrupts(green_timer_id);

	// don't start the colors until set_brightness is called
}



// caller must have called MSS_GPIO_init();
// because we don't want to re-init and change other gpio's setup by caller
void init_rgb_led(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b){
	MSS_GPIO_config((gpio_r = _gpio_r), MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_config((gpio_g = _gpio_g), MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_config((gpio_b = _gpio_b), MSS_GPIO_OUTPUT_MODE);
}

void on_pulse(void){
	if(pulse_direction == BRIGHTER) {
		if(master_brightness < full_brightness)
			set_brightness(master_brightness+1);
		else
			pulse_direction = DIMMER;
	} else {
		if(master_brightness > min_brightness)
			set_brightness(master_brightness-1);
		else
			pulse_direction = BRIGHTER;
	}
}

void pwm_red(){
	pwm_timer_handler(gpio_r, red_timer_id);
}

void pwm_green(){
	pwm_timer_handler(gpio_g, green_timer_id);
}

void pwm_blue(){
	pwm_timer_handler(gpio_b, blue_timer_id);
}

void pwm_timer_handler(uint32_t gpio, uint32_t timer_index)
{
    MSS_GPIO_clear_irq(gpio);
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

void set_color(uint8_t r, uint8_t g, uint8_t b){
  red = r; green = g; blue = g;
}

void set_brightness(uint8_t brightness){
  master_brightness = brightness;

  timer_setCompareVal(red_timer_id, red*(master_brightness/full_brightness));
  timer_setCompareVal(blue_timer_id, blue*(master_brightness/full_brightness));
  timer_setCompareVal(green_timer_id, green*(master_brightness/full_brightness));

  timer_enable(red_timer_id);
  timer_enable(green_timer_id);
  timer_enable(blue_timer_id);
}

// We could reverse this to make it more intuitive
// 0 = no pulse, 1(fast pulse)-2^32(slow pulse)
void set_pulse_rate(uint32_t rate){
   timer_setOverflowVal(pulse_timer_id, rate);
   if(rate)
     timer_enable(pulse_timer_id);
   else
	 timer_disable(pulse_timer_id);
}
