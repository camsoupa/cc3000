
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/rgb_led_driver/rgb_led.h"
#include <stdio.h>

int main(void)
{
	led_state * display_weather;
	led_state * display_air;

	MSS_GPIO_init();

   init_rgb_pwm(RED_GPIO_INT, GREEN_GPIO_INT, BLUE_GPIO_INT, PULSE_GPIO_INT);
   init_rgb_led(RED_GPIO, GREEN_GPIO, BLUE_GPIO);

   display_weather = create_led_state(
	0, 0, 255, //rgb
	0, //start brightness
	1000, //pulse rate
	8000, //duration
	TRANS_ON_MIN, //mode
	(led_state*)0 //next
   );

   display_air = create_led_state(
	0, 255, 0, //rgb
	0, //start brightness
	2000, //pulse rate
	4000, //duration
	TRANS_ON_MIN, //mode
	(led_state*)0  //next
   );

   // circularly linked-list intentional!
   // toggle between weather and air
   insert_led_state(display_weather, 0);
   insert_led_state(display_air, display_weather);
   display_air->next = display_weather;

   start_led_sequence();

   while(1) {}
}
