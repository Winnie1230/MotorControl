//#include<bcm2835.h>
#include<wiringPi.h>
//#define PIN1 RPI_GPIO_P2_12
//#define PIN2 RPI_GPIO_P2_33

#define PIN1 1
#define PIN2 23
int main(){
	bcm2835_init();
	bcm2835_gpio_fsel(PIN1,BCM2835_GPIO_FSEL_OUTP);
	//bcm2835_gpio_fsel(PIN2,BCM2835_GPIO_FSEL_OUTP);

	while(1){
	bcm2835_gpio_write(PIN1,LOW);
	bcm2835_gpio_write(PIN2,LOW);
	}
}
