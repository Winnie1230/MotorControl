#include<stdio.h>
#include<math.h>
#include<wiringPi.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdlib.h>

#define ENCODER_PIN 7
#define MOTOR1_PIN1 1 //pin12
#define MOTOR1_PIN2 //gpio control 
#define MOTOR2_PIN1 23 //pin33
#define MOTOR2_PIN2 //gpio control

int reso;
double rad;
double sample_time;

//car_speed
double speed;
double duration;
double vcmd;

//encoder
int encoder_pre;
int encoder_count;
int encoder_check;
struct timeval start, finish;

//PI control
double KP, KI, G, in, ERR;
int PWM;

void init(){
	reso = 5;
	rad = 360/(reso*2)*M_PI/180;
	sample_time = 1000;
	
	//encoder
	encoder_pre = 0;
	encoder_count = 0;
	encoder_check = 0;

	//PI control
	PWM = 100;
	KP = 200;
	KI = 1000;
	G = 0;
	in = 0;
	ERR = 0;

	//car_speed
	speed = 0;
	vcmd = 0;
	duration = 0;
}

void encoder();
double car_speed();
int pi_control(double speed);


int main(){
	wiringPiSetup();

	pinMode(ENCODER_PIN, INPUT);
	pinMode(MOTOR1_PIN1, PWM_OUTPUT);
	pinMode(MOTOR2_PIN1, PWM_OUTPUT);


	printf("Enter vcmd : ");
	scanf("%lf",&vcmd);

	while(1){
		pwmWrite(MOTOR1_PIN1, PWM);
		speed = car_speed();
		printf("current_speed = %lf\n", speed);
		PWM = pi_control(speed);
		printf("PWM = %d\n", PWM);
	}
}

void encoder(){
	int encoder_current = digitalRead(ENCODER_PIN);
	if(encoder_current != encoder_pre){
		if(encoder_check == 0){
			encoder_check = 1; //start to count
			gettimeofday(&start , NULL);
		}
		else{ //end to count
			encoder_check = 0;
			gettimeofday(&finish , NULL);
		}
	}
	else if(encoder_current == encoder_pre && encoder_check == 1){
		encoder_count ++;
	}
	encoder_pre = encoder_current;
}

double car_speed(){
	while(1){
		encoder();
		usleep(sample_time);
		if(encoder_count > 0 && encoder_check == 0)
			break;
	}

	duration = (finish.tv_sec - start.tv_sec) + (finish.tv_usec - start.tv_usec) * 0.000001;

	double speed = rad * 0.0625 / duration;
	encoder_count = 0;
	return speed;
}

int pi_control(double speed){
	ERR = vcmd - speed;
	G = ERR * KP;
	in = in + ERR * duration * KI;

	if(G > 1023)
		G = 1023;
	else if(G < -1023)
		G = -1023;

	if(in > 1023)
		in = 1023;
	else if(in < -1023)
		in = -1023;

	PWM = G + in;
	if(abs(PWM) >= 900)
		PWM = 900;
	else if(abs(PWM < 60))
		PWM = 60;

	return PWM;
}
