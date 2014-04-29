
#include "drivers/air_weather_display/air_weather_display.h"
#include "drivers/rgb_led_driver/rgb_led.h"

#define COLOR_CNT 6

int main(void)
{

	int i;
	init_weather_air_display();
	//insert_default_states();

	int colors[COLOR_CNT][3] = {
		{ 0, 255, 0 },
		{ 255, 255, 0 },
		{ 255, 126, 0 },
		{ 255, 0, 0 },
		{ 153, 0, 76 },
		{ 126, 0, 150 }
	};

	led_state * first = add_led_state(colors[0][0], colors[0][1], colors[0][2], 1, 1000, 2000);

	// add each of the air quality colors
	for(i=1; i < COLOR_CNT; i++) {
		add_led_state(colors[i][0], colors[i][1], colors[i][2], 1, 1000, 2000);
	}

	// make it start over when it ends
	//wait_state->next = first;

	// start the show!
	start_air_weather_display();

	while(1){}
}
