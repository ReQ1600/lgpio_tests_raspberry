#include <stdio.h>
#include <lgpio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

//defines
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
    //lgpio setup
    int lgpio_chip;
    int servo_gpio_handle;

    lgpio_chip = lgGpiochipOpen(0);
    if (lgpio_chip < 0) 
    {
        printf("Error opening GPIO chip\n");
        return -1;
    }

    servo_gpio_handle = lgGpioClaimOutput(lgpio_chip, GPIO_V2_LINE_FLAG_OPEN_DRAIN, SERVO_PIN, 0); //initialized low
    if (servo_gpio_handle < 0)
    {
        printf("Error claiming gpio pin\n");
        lgGpiochipClose(lgpio_chip);
        return -1;
    }
    
    //termios setup
    struct termios old_termios, new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    new_termios.c_lflag &= ~(ICANON | ECHO); //disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    //setting non blocking input flags
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    //main loop
    char c;
    int angle = 90;
    for(;;)
    {
        //close on q press
        c = getchar();
        if (c == 'q') break;
        printf("pulse width for %d = %d\n", angle, calculate_pulse_width(&angle));
        angle = (angle + 20) % 180;
        usleep(100000);
    }

    //on close
    //returning terminal to how it was
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);

    lgGpiochipClose(lgpio_chip);
    return 0;
}