#include<stdio.h>
#include<bcm2835.h>

#define MOTOR_PIN1 RPI_GPIO_P1_12
//#define MOTOR_PIN2 RPI_V2_GPIO_P1_16
#define PWM_CHANNEL 0
#define RANGE 1024

int main(){
	if(!bcm2835_init())
		return -1;

	bcm2835_gpio_fsel(MOTOR_PIN1, BCM2835_GPIO_FSEL_ALT5);
	//bcm2835_gpio_fsel(MOTOR_PIN2, BCM2835_GPIO_FSEL_OUTP);

	bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
	bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);
	
	/*while(1){
		bcm2835_pwm_set_data(PWM_CHANNEL, 512);
		bcm2835_delay(1);	
	}*/
	int direction = 1;
	int data = 1;
	
	while(1){
		if(data == 1)
			direction = 1;
		else if(data == RANGE-1)
			direction = -1;
		data += direction;
		bcm2835_pwm_set_data(PWM_CHANNEL, data);
		bcm2835_delay(1);
	}
	bcm2835_close();
	return 0;
}
