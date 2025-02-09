#include <stdio.h>
#include <wiringPi.h>

int main(void)
{
	if(wiringPiSetup() == -1)
	{
		printf("uh oh");
		return 1;
	}
	printf("a");
	pinMode(23, OUTPUT);
	printf("b");
	
	for(int i = 0; i < 1000; ++i)
	{
		printf("c");
		digitalWrite(0, LOW);
		printf("LOW");
		delay(200);
		digitalWrite(0, HIGH);
		printf("HIGH");
		delay(200);
	}

	printf("Hello World!");
	return 0;
}
