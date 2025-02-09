#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lgpio.h>

#define SERVO_PIN 17    // Change this to the GPIO pin you are using
#define CHIP 0          // GPIO chip (use 0 for Raspberry Pi)

#define SERVO_PERIOD 20000   // PWM period for 50 Hz (20ms)
#define MIN_PULSE_WIDTH 500  // Pulse width for 0 degrees (500 microseconds)
#define MAX_PULSE_WIDTH 2500 // Pulse width for 180 degrees (2500 microseconds)

#define SERVO_FREQUENCY 50  // 50 Hz frequency for the servo (standard for most servos)
#define SERVO_OFFSET 0      // No offset, 0 microseconds
#define SERVO_CYCLES 1      // Only one cycle for each movement

// Function to calculate the pulse width for a given angle
int calculate_pulse_width(int angle) {
    return MIN_PULSE_WIDTH + (angle * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / 180);
}

int main() {
    int h;
    unsigned pulse_width;

    // Open GPIO chip
    h = lgGpiochipOpen(CHIP);
    if (h < 0) {
        fprintf(stderr, "Failed to open GPIO chip\n");
        return EXIT_FAILURE;
    }

    // Set the pin as an output (for servo control)
    if (lgGpioClaimOutput(h, 0, SERVO_PIN, 0) < 0) {
        fprintf(stderr, "qFailed to claim GPIO pin\n");
        lgGpiochipClose(h);
        return EXIT_FAILURE;
    }

    // Calculate the pulse width for 85 degrees
    pulse_width = calculate_pulse_width(90);

    // Set the servo to 85 degrees using lgTxServo
    if (lgTxServo(h, SERVO_PIN, pulse_width, SERVO_FREQUENCY, SERVO_OFFSET, SERVO_CYCLES) < 0) {
        fprintf(stderr, "Failed to set servo position\n");
        lgGpiochipClose(h);
        return EXIT_FAILURE;
    }

    printf("Servo set to 85 degrees\n");

    sleep(2); // Hold the position for 2 seconds

    // Cleanup
    lgGpiochipClose(h);

    return EXIT_SUCCESS;
}
