#include <wiringPi.h>

#define PIN 1

int main(void)
{	
	if(wiringPiSetup() == -1) exit(1);

	pinMode(PIN, OUTPUT);
	digitalWrite(PIN, LOW);

	for(;;)
	{
		delay(500);
		digitalWrite(PIN, LOW);
		delay(500);
		digitalWrite(PIN, LOW);
	}
	return 0;
}
