/*
 * air_weather_display.c
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include "air_weather_display.h"
#include "../rgb_led_driver/rgb_led.h"

#define IN_RANGE(val, lower, upper) (val >= lower && val < upper)

void create_weather_air_states()
{
	set_led_state_1(format_led_state(0, 100, 255, 1000));
	set_led_state_2(format_led_state(255, 0, 100, 2000));
}

void start_air_weather_display(){
	start_led();
}

void init_weather_air_display()
{
	MSS_GPIO_init();
	create_weather_air_states();
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
	{	_r = 0; _g=228; _b=0; }
	else if(IN_RANGE(air_quality, 12.0, 35.5))
	{	_r = 255; _g=255; _b=0; }
	else if(IN_RANGE(air_quality, 35.5, 55.5 ))
	{	_r = 255; _g=126; _b=0; }
	else if(IN_RANGE(air_quality, 55.5, 150.5))
	{	_r = 255; _g=0; _b=0; }
	else if(IN_RANGE(air_quality, 150.5, 250.4))
	{	_r = 153; _g=0; _b=76; }
	else
	{	_r = 126; _g=0; _b=150; }

	set_led_state_1(format_led_state(_r, _g, _b, 1000));
}

void update_temperature(int temp)
{

}

void update_precipitation(uint8_t precipitation)
{
}
