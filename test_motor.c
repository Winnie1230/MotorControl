#include<bcm2835.h>
#include<stdio.h>

#define motor_pin_1 RPI_V2_GPIO_P1_12
#define motor_pin_2 RPI_V2_GPIO_P1_16
#define PWM_CHANNEL 0
#define RANGE 1024

int main(){
//wiringPiSetupGpio();
	if(!bcm2835_init())
		return 1;


	//bcm2835_gpio_fsel(motor_pin_1,BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(motor_pin_1,BCM2835_GPIO_FSEL_ALT5);
	bcm2835_gpio_fsel(motor_pin_2,BCM2835_GPIO_FSEL_OUTP);

	bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
	bcm2835_pwm_set_mode(PWM_CHANNEL,1,1);
	bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);
	
	int direction = 1;
	int data = 1;
	
	while(1){
		if(data == 1)
			direction = 1;
		else if(data == RANGE-1)
			direction = -1;
		data += direction;
		printf("data = %d\n",data);
		bcm2835_pwm_set_data(PWM_CHANNEL,data);
		bcm2835_delay(10);
		//bcm2835_gpio_write(motor_pin_1,HIGH);
		//bcm2835_gpio_write(motor_pin_2,LOW);
	}
	bcm2835_close();
	return 0;
}
