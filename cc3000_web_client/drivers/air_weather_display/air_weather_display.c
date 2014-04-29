/*
 * air_weather_display.c
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include "air_weather_display.h"
#include "../rgb_led_driver/rgb_led.h"

#define IN_RANGE(val, lower, upper) (val >= lower && val < upper)

// the number of demo air_quality values
#define RANGE_CNT 7

// the active led state
led_state * air_state;

// the pending led state
led_state * next_state;

void delay1(volatile int cnt){
	while(cnt){
		--cnt;
	}
}

/*
 * Display the range of colors for the
 * air quality groups
 *
 * You must first have initialized the
 */
void demo_air_quality_colors(){
	int i;
	float air_ranges[RANGE_CNT] = {
		0,
		15.0,
		30.0,
		50.0,
		70.0,
		160.0,
		250.0
	};

	for(i=0; i < RANGE_CNT; i++){
		update_air_quality(air_ranges[i]);
		delay1(10000000);
	}
}

/*
 * Add two shell led states to the rgb led driver
 */
void insert_default_states()
{
	// using 2 states: current and next
	// the next is just a holding tank for values
	// to be copied into the current state
	// when the led pulses off

	air_state     = (led_state *)malloc(sizeof(led_state));

	air_state->r = 0;
	air_state->g = 0;
	air_state->b = 0;
	air_state->brightness = 1;
	air_state->mode = TRANS_ON_MIN;
	air_state->pulse_rate_ms = 500;
	air_state->duration_ms = 1000;
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

	insert_led_state(air_state, 0);
	insert_led_state(next_state, air_state);
}

/*
 * initialize the rgb_led_driver gpios
 * the caller is reponsible to call MSS_GPIO_init()
 * prior to calling this function
 */
void init_weather_air_display()
{
	init_rgb_pwm(RED_GPIO_INT, GREEN_GPIO_INT, BLUE_GPIO_INT, PULSE_GPIO_INT);
	init_rgb_led(RED_GPIO, GREEN_GPIO, BLUE_GPIO);
}


/*
 * Begin the PWM based on the supplied states
 */
void start_air_weather_display(){
	start_led_sequence();
}

/*
 * Change the led color based on the air quality:
 *
 * See: http://www.airquality.utah.gov/aqp/currentconditions.php?id=slc
 * Air Quality Index:
 * Good	                    0 - 12.0 µg/m3	    0 - 0.059 ppm
 * Moderate	                12.1 - 35.4 µg/m3	0.06 - 0.075 ppm
 * Unhealthy for some...	35.5 - 55.4 µg/m3	0.076 - 0.095 ppm
 * Unhealthy	            55.5 - 150.4 µg/m3	0.096 - 0.115 ppm
 * Very Unhealthy	        150.5 - 250.4 µg/m3	0.116 - 0.374 ppm
 * Hazardous	            Above 250.5 µg/m3	Above 0.375 ppm
 */
void update_air_quality(float air_quality)
{
	uint8_t _r, _g, _b;


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

	// queue up the next state
	next_state->r = _r;
	next_state->b = _b;
	next_state->g = _g;
	next_state->pulse_rate_ms = 1000;
}
