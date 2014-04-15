/*
 * rgb_led.h
 *
 */



#ifndef RGB_LED_H_
#define RGB_LED_H_

#include <stdint.h>

void init_rgb_pwm(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b, uint8_t _gpio_i);

void init_rgb_led(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b);

void set_color(uint8_t r , uint8_t g, uint8_t b);

void set_brightness(uint8_t brightness);

void set_pulse_rate(uint32_t rate);

void pwm_timer_handler(uint32_t gpio, uint32_t timer_index);

void pwm_blue(void);

void pwm_red(void);

void pwm_green(void);

void on_pulse(void);

#endif /* RGB_LED_H_ */
