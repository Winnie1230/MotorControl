#include <bcm2835.h>
#include<stdio.h>

/* Blinks on RPi pin 7 */
#define PIN RPI_GPIO_P1_07

int main(int argc, char **argv) {

        if ( !bcm2835_init() ) {
                printf("Error");
                return 1;
        }

        /* Set the pin to be an output */
        bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);

        /* Blink LED */
        while (1) {
                /* Turn on LED */
                bcm2835_gpio_write(PIN, HIGH);
                /* Delay 500ms */
                delay(500);
                /* Turn off LED */
                bcm2835_gpio_write(PIN, LOW);
                /* Delay 500ms */
                delay(500);
        }

        return 0;
}
