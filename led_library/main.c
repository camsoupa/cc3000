
#include "drivers/air_weather_display/air_weather_display.h"

int main(void)
{
	init_weather_air_display();
	update_air_quality(1.1);

	while(1) {}
}
