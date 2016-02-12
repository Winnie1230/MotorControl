#include<stdio.h>
#include<wiringPi.h>

int main(){
	wiringPiSetup();
	pinMode(2 , INPUT);
	while(1){
		printf("%d\n" , digitalRead(2));
		delay(1000);
	}
}
