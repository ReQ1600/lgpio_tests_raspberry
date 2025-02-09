#include <stdio.h>
#include <lgpio.h>

int main(void) {
    int gpio_chip;
    gpio_chip = lgGpiochipOpen(0);  // Open the first GPIO chip (chip 0)

    if (gpio_chip < 0) {
        printf("Error opening GPIO chip\n");
        return 1;
    }

    // Declare the structure as a pointer to struct lgChipInfo_s
    struct lgChipInfo_s chip_info;

    // Get the GPIO chip information
    int result = lgGpioGetChipInfo(gpio_chip, &chip_info);
    if (result < 0) {
        printf("Error fetching chip information\n");
        lgGpiochipClose(gpio_chip);
        return 1;
    }

    // Print chip information
    printf("GPIO Chip Information:\n");
    printf("Chip name: %s\n", chip_info.name);
    printf("Chip label: %s\n", chip_info.label);

    // Close the GPIO chip
    lgGpiochipClose(gpio_chip);

    return 0;
}

