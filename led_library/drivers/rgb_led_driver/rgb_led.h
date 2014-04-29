/*
 * rgb_led.h
 *
 * Version 2
 */



#ifndef RGB_LED_H_
#define RGB_LED_H_

#include <stdint.h>
#include "../mss_gpio/mss_gpio.h"

uint32_t format_led_state(uint8_t r, uint8_t g, uint8_t b, uint8_t pulse);

void set_led_state_1(uint32_t val);
void set_led_state_2(uint32_t val);

uint32_t get_led_state_1(void);
uint32_t get_led_state_2(void);

void start_led(void);
void stop_led(void);

#endif /* RGB_LED_H_ */
