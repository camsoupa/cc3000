/*
 * rgb_led.c
 *
 */

#include "./rgb_led.h"
#include "../mss_gpio/mss_gpio.h"

#define DIMMER  0
#define BRIGHTER  1

uint8_t rgb_gpio;

//0-100
uint8_t master_brightness = 100;
uint8_t full_brightness = 100;
uint8_t min_brightness = 0;
uint8_t pulse_direction;



uint8_t red;
uint8_t green;
uint8_t blue;

// caller must have called MSS_GPIO_init();
// because we don't want to re-init and change other gpio's setup by caller
void init_rgb_led(uint8_t gpio){
  rgb_gpio = gpio;
  MSS_GPIO_config(gpio, MSS_GPIO_OUTPUT_MODE);
}

void on_pulse(void){
	if(pulse_direction == BRIGHTER) {
		  if(master_brightness < full_brightness)
			set_brightness(++master_brightness);
		  else
			 pulse_direction = DIMMER;
	} else {
		  if(master_brightness > min_brightness)
		    set_brightness(--master_brightness);
		  else
			 pulse_direction = BRIGHTER;
	}
}

void set_color(uint8_t r, uint8_t g, uint8_t b){


}

void set_brightness(uint8_t brightness){

}

void set_pulse_rate(uint8_t rate){
   //setup pulse interrupt
}
