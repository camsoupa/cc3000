/*
 * rgb_led.h
 *
 */



#ifndef RGB_LED_H_
#define RGB_LED_H_

#include <stdint.h>

void set_color(uint8_t r , uint8_t g, uint8_t b);

void set_brightness(uint8_t brightness);

void set_pulse_rate(uint8_t rate);

#endif /* RGB_LED_H_ */
