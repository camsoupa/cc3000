/*
 * air_weather_display.h
 *
 */

#ifndef AIR_WEATHER_DISPLAY_H_
#define AIR_WEATHER_DISPLAY_H_

#include <stdint.h>

void update_led_air_quality(float air_quality);
void update_led_temperature(int temp);
void update_led_precipitation(uint8_t precipitation);

#endif /* AIR_WEATHER_DISPLAY_H_ */
