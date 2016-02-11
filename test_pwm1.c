#include<wiringPi.h>
#include<softPwm.h>

//gcc -o test test.c -lwiringPi -lpthread
#define PIN 1 //pin 12
#define RANGE 100

int main(){
	wiringPiSetup();
	pinMode(PIN,PWM_OUTPUT); //0(off)~100(fully on)

	softPwmCreate(PIN, 100 ,RANGE);
	
	while(1){
		softPwmWrite(PIN,50);
	}
	return 0;
}
