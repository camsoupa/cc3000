/*
 * rgb_led.h
 *
 */



#ifndef RGB_LED_H_
#define RGB_LED_H_

#include <stdint.h>
#include "../mss_gpio/mss_gpio.h"

#define LED_STATE_GPIO_INT MSS_GPIO_14

// pwm interrupts
// the fpga configured interrupts for the timer modules
#define RED_GPIO_INT   MSS_GPIO_5
#define GREEN_GPIO_INT MSS_GPIO_6
#define BLUE_GPIO_INT  MSS_GPIO_7
#define PULSE_GPIO_INT MSS_GPIO_8

// the fpga configured pins for led control
// TODO: add these to SoftConsole
#define RED_GPIO   MSS_GPIO_11
#define GREEN_GPIO MSS_GPIO_12
#define BLUE_GPIO  MSS_GPIO_13

#define HZ_PER_MS 1000


// led state modes
#define	FREE_WHEN_DONE  0x2
#define	TRANS_ON_MAX    0x4
#define TRANS_ON_MIN    0x8

typedef struct led_state {
	uint8_t  r, g, b, brightness, mode;
	uint32_t pulse_rate_ms;
	uint32_t duration_ms;
	struct led_state * next;
} led_state;


void init_rgb_pwm(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b, uint8_t _gpio_i);

void init_rgb_led(uint8_t _gpio_r, uint8_t _gpio_g, uint8_t _gpio_b);

void set_color(uint8_t r , uint8_t g, uint8_t b);

void set_brightness(uint8_t brightness);

void set_pulse_rate(uint32_t rate);

void pwm_timer_handler(uint32_t gpio, uint32_t timer_index);

void pwm_blue(void);

void pwm_red(void);

led_state *
create_led_state(
	uint8_t  r, uint8_t g, uint8_t b,
	uint8_t  brightness,
	uint32_t pulse_rate_ms,
	uint32_t duration_ms,
	uint8_t  mode);

void insert_led_state(led_state * state, led_state * after);

void start_led_sequence(void);

void start_led_state_timer(led_state * ls);

void transition_to_next_led_state(void);

void pwm_green(void);

void on_pulse(void);

void start_led_sequence(void);

void transition_to_next_state(void);

void on_led_state_duration_reached(void);

#endif /* RGB_LED_H_ */
