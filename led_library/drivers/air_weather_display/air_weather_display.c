/*
 * air_weather_display.c
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include "air_weather_display.h"
#include "../rgb_led_driver/rgb_led.h"

led_state * weather_state;
led_state * air_state;

void init_weather_air_display()
{
	weather_state = (led_state *)malloc(sizeof(led_state));
	air_state     = (led_state *)malloc(sizeof(led_state));

	MSS_GPIO_init();

	init_rgb_pwm(RED_GPIO_INT, GREEN_GPIO_INT, BLUE_GPIO_INT, PULSE_GPIO_INT);
	init_rgb_led(RED_GPIO, GREEN_GPIO, BLUE_GPIO);

	weather_state->r = 0;
	weather_state->g = 0;
	weather_state->b = 0;
	weather_state->brightness = 0;
	weather_state->mode = TRANS_ON_MIN;
	weather_state->pulse_rate_ms = 0;
	weather_state->duration_ms = 0;
	weather_state->next = 0;

	air_state->r = 0;
	air_state->g = 0;
	air_state->b = 255;
	air_state->brightness = 0;
	air_state->mode = TRANS_ON_MIN;
	air_state->pulse_rate_ms = 500;
	air_state->duration_ms = 1000;
	air_state->next = 0;

	// circularly linked-list intentional!
	// toggle between weather and air
	//insert_led_state(weather_state, 0);
	//insert_led_state(air_state, weather_state);
	//air_state->next = weather_state;
	insert_led_state(air_state, 0);
	start_led_sequence();
}

void update_air_quality(float air_quality)
{
	air_state->g = 255;
	air_state->b = 0;
	air_state->pulse_rate_ms = 1000;
	air_state->duration_ms = 4000;
	start_led_sequence();
}

void update_temperature(int temp)
{

}

void update_precipitation(uint8_t precipitation)
{
}
