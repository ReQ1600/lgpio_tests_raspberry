#include <stdio.h>
#include <lgpio.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

//defines
#define SERVO_PIN 17
#define PWM_FREQ 50 //Hz
#define MIN_PULSE 900 //0deg
#define MAX_PULSE 2100 //180deg
#define PERIOD 20000 //20ms
#define MAX_ANGLE 120
#define ANGLE_INCREMENT 5

int calculate_pulse_width(uint8_t *angle)
{
    return MIN_PULSE + ((int)(*angle) * (MAX_PULSE - MIN_PULSE) / MAX_ANGLE);
}

//returns true on failure
bool servo_set_position(int* chip_handle, uint8_t* angle)
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

//returns true if q is pressed
bool interpret_key_press(char *c, uint8_t *current_servo_pos, bool *angle_changed)
{
    switch (*c)
    {
    case 'a':
        (*current_servo_pos)-= ANGLE_INCREMENT;
        break;
    case 'd':
        (*current_servo_pos)+= ANGLE_INCREMENT;
        break;
    case 'q':
        return true;
    default:
        *angle_changed = false;
        return false;
    }
    *angle_changed = true;
    return false;
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
    new_termios.c_cc[VMIN] = 1; //read only 1 character
    new_termios.c_cc[VTIME] = 0; //no timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    //setting non blocking input flags
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    //main loop
    char c;
    bool angle_changed = true;
    uint8_t angle = 0;

    for(;;)
    {
        //just dont hold it for too long for some reason it just refuses to buffer only 1 character or to flush its buffer
        c = getchar();
        
        if (interpret_key_press(&c, &angle, &angle_changed)) break;
        printf("rangle: %d\n", angle);

        
        if (angle > MAX_ANGLE) angle = MAX_ANGLE;
        if (angle_changed)
        {
            if (servo_set_position(&lgpio_chip, &angle))
            {
                terminal_return(&old_termios, &flags);
                return -1;
            }
        }
        usleep(200000);
    }

    //on close
    //returning terminal to how it was
    terminal_return(&old_termios, &flags);

    lgGpiochipClose(lgpio_chip);
    return 0;
}