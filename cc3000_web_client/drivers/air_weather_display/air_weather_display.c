/*
 * air_weather_display.c
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "../mss_gpio/mss_gpio.h"
#include "air_weather_display.h"
#include "../rgb_led_driver/rgb_led.h"

//led_state * display_weather;
//led_state * display_air;

// pwm interrupts
// the fpga configured interrupts for the timer modules
#define RED_GPIO_INT   MSS_GPIO_5
#define GREEN_GPIO_INT MSS_GPIO_6
#define BLUE_GPIO_INT  MSS_GPIO_7
#define PULSE_GPIO_INT MSS_GPIO_8

// the fpga configured pins for led control
// TODO: add these to SoftConsole
#define RED_GPIO   MSS_GPIO_11
#define GREEN_GPIO MSS_GPIO_12
#define BLUE_GPIO  MSS_GPIO_13

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

void init_weather_air_display()
{
	//display_weather = (led_state *)malloc(sizeof(led_state));
	//display_air     = (led_state *)malloc(sizeof(led_state));
	init_rgb_pwm(RED_GPIO_INT, GREEN_GPIO_INT, BLUE_GPIO_INT, PULSE_GPIO_INT);
	init_rgb_led(RED_GPIO, GREEN_GPIO, BLUE_GPIO);
	set_color(255,255,255);
	set_brightness(1);
	set_pulse_rate(100);
}

void update_air_quality(float air_quality)
{
	set_color(0,255,0);
}

void update_temperature(int temp)
{

}

void update_precipitation(uint8_t precipitation)
{
}
