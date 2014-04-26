
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/rgb_led_driver/rgb_led.h"
#include <stdio.h>

// pwm interrupts
// the fpga configured interrupts for the timer modules
#define RED_GPIO_INT   MSS_GPIO_5
#define GREEN_GPIO_INT MSS_GPIO_6
#define BLUE_GPIO_INT  MSS_GPIO_7
#define PULSE_GPIO_INT MSS_GPIO_8

__attribute__ ((interrupt)) void GPIO5_IRQHandler(void){
	pwm_red();
}

__attribute__ ((interrupt)) void GPIO6_IRQHandler(void){
	pwm_green();
}

__attribute__ ((interrupt)) void GPIO7_IRQHandler(void){
	pwm_blue();
}

__attribute__ ((interrupt)) void GPIO8_IRQHandler(void){
	on_pulse();
}

__attribute__ ((interrupt)) void GPIO14_IRQHandler(void){
	on_led_state_duration_reached();
}


// the fpga configured pins for led control
// TODO: add these to SoftConsole
#define RED_GPIO   MSS_GPIO_11
#define GREEN_GPIO MSS_GPIO_12
#define BLUE_GPIO  MSS_GPIO_13

extern void* g_pfnVectors;

void delay(uint32_t cnt){
	while(cnt){
		--cnt;
	}
}

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
	display_weather //next
   );

   // circularly linked-list intentional!
   // toggle between weather and air
   insert_led_state(display_weather, 0);
   insert_led_state(display_air, display_weather);

   start_led_sequence();

   while(1) {}
}
