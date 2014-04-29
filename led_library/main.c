
#include "drivers/air_weather_display/air_weather_display.h"
#include "drivers/rgb_led_driver/rgb_led.h"

#define RANGE_CNT 7

void delay(int cnt){
	while(cnt){
		--cnt;
	}
}

int main(void)
{
	int i;
	float air_ranges[RANGE_CNT] = {
		0,
		15.0,
		30.0,
		50.0,
		70.0,
		160.0,
		250.0
	};

	init_weather_air_display();
	start_air_weather_display();
	for(i=0; i < RANGE_CNT; i++){
		delay(500000);
		update_air_quality(air_ranges[RANGE_CNT]);
	}
	while(1){}
}
