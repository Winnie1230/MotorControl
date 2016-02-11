#include <bcm2835.h>
int main(int argc, char** argv) {
	if (!bcm2835_init())
		return 1;
	bcm2835_gpio_fsel(18,BCM2835_GPIO_FSEL_ALT5 );    
	bcm2835_pwm_set_clock(2);
	bcm2835_pwm_set_mode(0, 1, 1);
	bcm2835_pwm_set_range(0,2);
	bcm2835_pwm_set_data(0,1); 

	return 0;
}
