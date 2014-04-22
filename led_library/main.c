
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
   MSS_GPIO_init();


   printf("hello world! %d\r\n", (uint32_t)g_pfnVectors);

   init_rgb_pwm(RED_GPIO_INT, GREEN_GPIO_INT, BLUE_GPIO_INT, PULSE_GPIO_INT);
   init_rgb_led(RED_GPIO, GREEN_GPIO, BLUE_GPIO);
   set_color(255,255,255);
   set_brightness(100);
   set_pulse_rate(000);
   uint8_t chan_red = 0;
   uint8_t chan_blue = 150;
   uint8_t chan_green = 255;
   while(1) {
    // set_color(chan_red, chan_green, chan_blue);
     //delay(100000);
	 chan_red++; chan_green--;
   }
}
