/*
 * air_weather_display.h
 *
 */

#ifndef AIR_WEATHER_DISPLAY_H_
#define AIR_WEATHER_DISPLAY_H_

#include <stdint.h>

void init_weather_air_display(void);
void update_air_quality(float air_quality);
void update_temperature(int temp);
void update_precipitation(uint8_t precipitation);
void create_weather_air_states(void);
void start_air_weather_display(void);


#endif /* AIR_WEATHER_DISPLAY_H_ */
