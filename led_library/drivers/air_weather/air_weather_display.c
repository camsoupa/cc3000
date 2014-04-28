/*
 * air_weather_display.c
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "air_weather_display.h"
#include "../rgb_led_driver/rgb_led.h"

led_state * display_weather;
led_state * display_air;

void init_weather_air()
{
	display_weather = (led_state *)malloc(sizeof(led_state));
	display_air     = (led_state *)malloc(sizeof(led_state));
}

void update_air_quality(float air_quality)
{
	display_air->
}

void update_temperature(int temp)
{

}

void update_precipitation(uint8_t precipitation)
{
}
