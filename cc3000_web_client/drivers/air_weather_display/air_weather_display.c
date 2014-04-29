/*
 * air_weather_display.c
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include "air_weather_display.h"
#include "../rgb_led_driver/rgb_led.h"

#define IN_RANGE(val, lower, upper) (val >= lower && val < upper)

led_state * air_state;
led_state * next_state;

void insert_default_states()
{
	air_state     = (led_state *)malloc(sizeof(led_state));

	air_state->r = 0;
	air_state->g = 0;
	air_state->b = 0;
	air_state->brightness = 1;
	air_state->mode = TRANS_ON_MIN;
	air_state->pulse_rate_ms = 1000;
	air_state->duration_ms = 2000;
	air_state->next = 0;

	next_state = (led_state *)malloc(sizeof(led_state));

	next_state->r = 0;
	next_state->g = 0;
	next_state->b = 0;
	next_state->brightness = 1;
	next_state->mode = TRANS_ON_MIN;
	next_state->pulse_rate_ms = 1000;
	next_state->duration_ms = 2000;
	next_state->next = 0;

	// circularly linked-list intentional!
	// toggle between weather and air
	//insert_led_state(weather_state, 0);
	//insert_led_state(air_state, weather_state);
	//air_state->next = weather_state;
	insert_led_state(air_state, 0);
	insert_led_state(next_state, air_state);
}

void start_air_weather_display(){
	start_led_sequence();
}

void init_weather_air_display()
{
	init_rgb_pwm(RED_GPIO_INT, GREEN_GPIO_INT, BLUE_GPIO_INT, PULSE_GPIO_INT);
	init_rgb_led(RED_GPIO, GREEN_GPIO, BLUE_GPIO);
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

	next_state->r = _r;
	next_state->b = _b;
	next_state->g = _g;
	next_state->pulse_rate_ms = 1000;
	//led_state_values_changed();
	//set_led_state(air_state);
}

void update_temperature(int temp)
{

}

void update_precipitation(uint8_t precipitation)
{
}
