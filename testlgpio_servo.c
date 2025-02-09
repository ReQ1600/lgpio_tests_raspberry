#include <stdio.h>
#include <lgpio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>


//TODO: add making terminal normal again when errors occur
//defines
#define SERVO_PIN 17
#define PWM_FREQ 50 //Hz
#define MIN_PULSE 900 //0deg
#define MAX_PULSE 2100 //180deg
#define PERIOD 20000 //20ms

int calculate_pulse_width(int *angle)
{
    return MIN_PULSE + (*angle * (MAX_PULSE - MIN_PULSE) / 120);
}

//returns true on failure
bool servo_set_position(int* chip_handle, int* angle)
{
    if (lgTxServo(*chip_handle, SERVO_PIN, calculate_pulse_width(angle), PWM_FREQ, 0, 10) < 0) 
    {
        fprintf(stderr, "Failed to set Servo position\n");
        lgGpiochipClose(*chip_handle);
        return true;
    }
    return false;
}

void terminal_return(struct termios* old, int* flags)
{
    tcsetattr(STDIN_FILENO, TCSANOW, old);
    fcntl(STDIN_FILENO, F_SETFL, *flags & ~O_NONBLOCK);
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

    servo_gpio_handle = lgGpioClaimOutput(lgpio_chip, 0, SERVO_PIN, 0); //initialized low
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
    int angle;

    for(;;)
    {
        //close on q press
        c = getchar();
        if (c == 'q') break;

        angle = 0;
        if (servo_set_position(&lgpio_chip, &angle))
        {
            terminal_return(&old_termios, &flags);
            return -1;
        }
        sleep(1);
        angle = 120;
        if (servo_set_position(&lgpio_chip, &angle))
        {
            terminal_return(&old_termios, &flags);
            return -1;
        }
        sleep(1);
    }

    //on close
    //returning terminal to how it was
    terminal_return(&old_termios, &flags);

    lgGpiochipClose(lgpio_chip);
    return 0;
}