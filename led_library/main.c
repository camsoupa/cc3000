
#include "drivers/air_weather_display/air_weather_display.h"

void delay(int ms) {
	int v = ms*1000;
	while(v) { v--; }
}

int main(void)
{
	int air = 0;
	init_weather_air_display();
	while(1) {

		update_air_quality(air);
		air = air + 10;
		delay(20000);
	}
}
