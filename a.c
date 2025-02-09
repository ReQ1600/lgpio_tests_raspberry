#include <stdio.h>
#include <lgpio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define SERVO_PIN 17
#define PWM_FREQ 50 //Hz
#define MIN_PULSE 500 //0deg
#define MAX_PULSE 2500 //180deg
#define PERIOD 20000 //20ms

int calculate_pulse_width(int *angle)
{
    return MIN_PULSE + (*angle * (MAX_PULSE - MIN_PULSE) / 180);
} 

int main(void)
{
    int angle = 20;
    printf("chuj %d\n", calculate_pulse_width(&angle));
    return 0;
}