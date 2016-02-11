#include<bcm2835.h>

#define PIN1 RPI_GPIO_P1_12
//#define PIN2 RPI_GPIO_P1_16

int main(){
	bcm2835_init();
	bcm2835_gpio_fsel(PIN1,BCM2835_GPIO_FSEL_OUTP);
	//bcm2835_gpio_fsel(PIN2,BCM2835_GPIO_FSEL_OUTP);

	while(1){
	bcm2835_gpio_write(PIN1,HIGH);
	//bcm2835_gpio_write(PIN2,HIGH);
	}
}
