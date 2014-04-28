
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/rgb_led_driver/rgb_led.h"
#include <stdio.h>

int main(void)
{
	led_state weather_state;
	led_state air_state;

	MSS_GPIO_init();

	init_rgb_pwm(RED_GPIO_INT, GREEN_GPIO_INT, BLUE_GPIO_INT, PULSE_GPIO_INT);
	init_rgb_led(RED_GPIO, GREEN_GPIO, BLUE_GPIO);

	weather_state.r = 0;
	weather_state.g = 0;
	weather_state.b = 255;
	weather_state.brightness = 0;
	weather_state.mode = TRANS_ON_MIN;
	weather_state.pulse_rate_ms = 1000;
	weather_state.duration_ms = 8000;
	weather_state.next = 0;

	air_state.r = 0;
	air_state.g = 255;
	air_state.b = 0;
	air_state.brightness = 0;
	air_state.mode = TRANS_ON_MIN;
	air_state.pulse_rate_ms = 2000;
	air_state.duration_ms = 4000;
	air_state.next = 0;

	// circularly linked-list intentional!
	// toggle between weather and air
	insert_led_state(&weather_state, 0);
	insert_led_state(&air_state, &weather_state);
	air_state.next = &weather_state;

	start_led_sequence();

	while(1) {}
}
