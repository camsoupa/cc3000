/*
 * rgb_led.c
 *
 * Version 2
 */

#include <stdio.h>
#include <stdlib.h>
#include "rgb_led.h"

#define LED_CONTROL_REG 0x40050000
#define LED_STATE_1_CONTROL_REG 0x40050004
#define LED_STATE_2_CONTROL_REG 0x40050008

uint32_t * led_cntrl = ((uint32_t *) LED_CONTROL_REG);
uint32_t * led_state_1 = ((uint32_t *) LED_STATE_1_CONTROL_REG);
uint32_t * led_state_2 = ((uint32_t *) LED_STATE_2_CONTROL_REG);

uint32_t format_led_state(uint8_t r, uint8_t g, uint8_t b, uint8_t pulse) {
	return ((((uint32_t)pulse) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | (uint32_t)b);
}

void set_led_state_1(uint32_t val){
	*led_state_1 = val;
}

void set_led_state_2(uint32_t val){
	*led_state_2 = val;
}

void start_led(){
	*led_cntrl  = 1;
}

void stop_led() {
	*led_cntrl  = 0;
}


