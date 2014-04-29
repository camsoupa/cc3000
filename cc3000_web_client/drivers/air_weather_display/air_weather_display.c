/*
 * air_weather_display.c
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "../mss_gpio/mss_gpio.h"
#include "air_weather_display.h"
#include "../rgb_led_driver/rgb_led.h"


#define IN_RANGE(val, lower, upper) (val >= lower && val < upper)

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
	uint8_t _r, _g, _b;

	//Good	0 - 12.0 µg/m3	0 - 0.059 ppm
	//Moderate	12.1 - 35.4 µg/m3	0.06 - 0.075 ppm
	//Unhealthy for Sensitive Groups	35.5 - 55.4 µg/m3	0.076 - 0.095 ppm
	//Unhealthy	55.5 - 150.4 µg/m3	0.096 - 0.115 ppm
	//Very Unhealthy	150.5 - 250.4 µg/m3	0.116 - 0.374 ppm
	//Hazardous	Above 250.5 µg/m3	Above 0.375 ppm

	//good rgb(0,228,0);
	if(IN_RANGE(air_quality, 0, 12.0))
	{
		printf("GREEN!\r\n");
		_r = 0; _g=228; _b=0;
	}
	else if(IN_RANGE(air_quality, 12.0, 35.5))
	{
		printf("GREEN!\r\n");
		_r = 255; _g=255; _b=0;
	}
	else if(IN_RANGE(air_quality, 35.5, 55.5 ))
	{
		printf("GREEN!\r\n");
		_r = 255; _g=126; _b=0;
	}
	else if(IN_RANGE(air_quality, 55.5, 150.5))
	{
		printf("GREEN!\r\n");
		_r = 255; _g=0; _b=0;
	}
	else if(IN_RANGE(air_quality, 150.5, 250.4))
	{
		printf("GREEN!\r\n");
		_r = 153; _g=0; _b=76;
	}
	else
	{
		printf("GREEN!\r\n");
		_r = 126; _g=0; _b=150;
	}

	set_color(_r,_g,_b);
	set_pulse_rate(1000);
}

void update_temperature(int temp)
{

}

void update_precipitation(uint8_t precipitation)
{
}
