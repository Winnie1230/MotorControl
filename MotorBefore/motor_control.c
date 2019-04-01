#include<iostream>
#include<math.h>
#include<wiringPi.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdint.h>
#include<signal.h>
#include "rs232.h"
#include<string.h>

#define ENCODER_PIN1 2 //pin13
#define ENCODER_PIN2 0 //pin11

using namespace std;

int reso , sample_time;
double rad;

//car_speed
double speed1, speed2;
double duration;
struct timeval check;

//encoder
int encoder_pre;
int encoder_count;
int encoder_check;
int diff;
int check_num; //read three 1 or 0 then start/end counting
int checktime; //encoder maximum time
int step;
struct timeval start, finish;

//PI control
//double KP_1 , KP_2 , KI_1 , KI_2 , KD_1 , KD_2 , G_1 , G_2 , in_1 , in_2 , pre_ERR;
double KP_1 , KP_2 , KI_1 , KI_2 , G_1 , G_2 , in_1 , in_2;
int PWM1, PWM2;

//send to arduino
int cport_nr = 24;
int bdrate = 115200;
int BUF_SIZE = 20;
char send[20];

void init(){
	reso = 9;
	step = 9;
	rad = 360/(reso*2)*M_PI/180 * step;
	sample_time = 1000;
	
	//encoder
	encoder_pre = 0;
	encoder_count = 0; //end flag
	encoder_check = 0;
	diff = -1;
	check_num = 2;
	checktime = 1.2; //1.5s

	//PI control
	PWM1 = 0;		PWM2 = 0;
	KP_1 = 12;		KP_2 = 20;
	KI_1 = 25;		KI_2 = 20;
	//KD_1 = 0;		KD_2 = 0;
	G_1 = 0;		G_2 = 0;
	in_1 = 0; 		in_2 = 0;
	//pre_ERR = 0;

	//car_speed
	speed1 = 0;//motor1
	speed2 = 0;//motor2
	duration = 0;
}

void encoder(int);
double car_speed(int , int);
int pi_control(int , double , double);
void sighandler(int);

//send to arduino
void int2str(int, char*);
void result(int , int , char[]);

int main(){
	
	wiringPiSetup();
	pinMode(ENCODER_PIN1 , INPUT);
	pinMode(ENCODER_PIN2 , INPUT);
	init();
	double vcmd1, vcmd2;
	
	char mode[] = {'8' , 'N' , '1' , 0};
	unsigned char str_recv[BUF_SIZE];
	
	signal(SIGINT , sighandler);

	if(RS232_OpenComport(cport_nr, bdrate, mode)){
    		printf("Can not open comport\n");
    		return(0);
  	}
        usleep(20000);

	//RS232_flushRXTX(cport_nr);
	//usleep(10000);	

	printf("vcmd1 vcmd2 :");
	scanf("%lf %lf" , &vcmd1 , &vcmd2);
	//vcmd1 = 3;
	//vcmd2 = 3;	

	while(1){
       		result(PWM1 , PWM2 , send);
        	RS232_cputs(cport_nr, send); // sends string on serial
        	printf("Sent to Arduino: '%s'\n", send);
        	usleep(500000);  /* waits for reply 1000ms */
        	int n = RS232_PollComport(cport_nr, str_recv, (int)BUF_SIZE);
        	if(n > 0){
      			str_recv[n] = 0;   /* always put a "null" at the end of a string! */
      			printf("Received %i bytes: '%s'\n", n, (char *)str_recv);
 			RS232_flushRXTX(cport_nr); 
			usleep(100000);
		}     	
    		//usleep(5000000);  /* sleep for 5 Second */
		speed1 = car_speed(PWM1 , 1);
		PWM1 = pi_control(1 , vcmd1 , speed1);
		speed2 =  car_speed(PWM2 ,2);
		PWM2 = pi_control(2 , vcmd2 , speed2);
		//usleep(1000);
		//printf("%lf %d\n", speed1 , PWM1);
		//printf("%lf %d\n" , speed2 , PWM2);
		printf("%lf %d %lf %d\n", speed1 , PWM1, speed2 , PWM2);
	}
	return 0;
}

void encoder(int pin){
	int encoder_current;
	if(pin == 1)
		encoder_current = digitalRead(ENCODER_PIN1);
	else 
		encoder_current = digitalRead(ENCODER_PIN2);

	//printf("%d " , encoder_current);

	/*if(encoder_current != encoder_pre){
		if (encoder_check == 0){
			encoder_check = 1;
			gettimeofday(&start , NULL);
		}
		else{
			encoder_check = 0;
			gettimeofday(&finish , NULL);
		}
	}
	else if(encoder_current == encoder_pre && encoder_check == 1)
		encoder_count ++;*/

	if(encoder_current != encoder_pre){
		if(diff == -1)
			diff = 0;
		else if((diff >= 0 && diff < check_num) && encoder_check == 1) //noise
			diff = -1;
	}
	else if(encoder_current == encoder_pre){
		if(diff >= 0 && diff < check_num)
			diff ++;
		if(diff >= check_num){
			if(encoder_check == 0){
				encoder_check = 1; //start to count
				if(encoder_count == 0)
					gettimeofday(&start , NULL);
				diff = -1;
			}
			else{//encoder_check = 1
				encoder_check = 0; //end counting
				//gettimeofday(&finish , NULL);
				diff = -1;
				encoder_count ++;//end flag
				if(encoder_count == step - 1)
					gettimeofday(&finish , NULL);
			}
		}	
	}
	encoder_pre = encoder_current;
}

double car_speed(int PWM , int pin){
	double time;
	struct timeval check_s , check_f;
	gettimeofday(&check_s , NULL);
	while(1){
		encoder(pin);
		usleep(sample_time);
		gettimeofday(&check_f , NULL);
		double time = (check_f.tv_sec - check_s.tv_sec) + (check_f.tv_usec - check_s.tv_usec) * 0.000001;
		if (encoder_count >= step - 1)
			break;
		if(time > checktime){ //speed == 0
			duration = checktime;
			return 0;
		}
		//if(encoder_count > 0 && encoder_check == 0)
			//break;
	}

	duration = (finish.tv_sec - start.tv_sec) + (finish.tv_usec - start.tv_usec) * 0.000001;

	printf("\nduration = %lf\n" , duration);
	double speed = rad * 0.0675 / duration;
	encoder_count = 0;
	return speed;
}

int pi_control(int num , double vcmd, double speed){
	int PWM = 0;
	double ERR , G , in , D;
	
	ERR = vcmd - speed;
	//printf("ERR = %lf\n" , ERR);

	if(num == 1){
		G_1 = ERR * KP_1;
		in_1 = in_1 + ERR * duration * KI_1;
		/*if(speed == 0)
			D = 0;
		else
			D = (ERR - pre_ERR) * KD_1 / duration;	*/	

		G = G_1;
		in = in_1;
	}
	else{
		G_2 = ERR * KP_2;
		in_2 = in_2 + ERR * duration * KI_2;
		/*if(speed == 0)
			D = 0;
		else
			D = (ERR - pre_ERR) * KD_2 / duration;*/
	
		G = G_2;
		in = in_2;
	}

	if(G > 255)
		G = 255;
	else if(G < -255)
		G = -255;

	if(in > 255)
		in = 255;
	else if(in < -255)
		in = -255;
	
	/*if(D > 255)
		D = 255;
	else if(D < -255)
		D = -255;*/

	PWM = G + in;
	if(abs(PWM) >= 150)
		PWM = 150;
	else 
		PWM = abs(PWM);

	//pre_ERR = ERR;	

	return PWM;
}

void sighandler(int signum){
	result(0 , 0 , send);
	RS232_cputs(cport_nr, send);
	usleep(100000);
	exit(1);
}

void result(int pwm1 , int pwm2 , char s[20]){
        char pwm1_c[10], pwm2_c[10];
        int2str(pwm1 , pwm1_c);
         if(strlen(pwm1_c) == 1){
                pwm1_c[3] = '\0';
                pwm1_c[2] = pwm1_c[0];
                pwm1_c[1] = '0';
                pwm1_c[0] = '0';
        }
        else if(strlen(pwm1_c) == 2){
                pwm1_c[3] = '\0';
                pwm1_c[2] = pwm1_c[1];
                pwm1_c[1] = pwm1_c[0];
                pwm1_c[0] = '0';
        }

        int2str(pwm2 , pwm2_c);
         if(strlen(pwm2_c) == 1){
                pwm2_c[3] = '\0';
                pwm2_c[2] = pwm2_c[0];
                pwm2_c[1] = '0';
                pwm2_c[0] = '0';
        }
         else if(strlen(pwm2_c) == 2){
                pwm2_c[3] = '\0';
                pwm2_c[2] = pwm2_c[1];
                pwm2_c[1] = pwm2_c[0];
                pwm2_c[0] = '0';
        }

        char gpio1[] = "0";
        char gpio2[] = "1";
        strcpy(send , gpio1);
        strcat(s , pwm1_c);
        strcat(s , gpio2);
        strcat(s , pwm2_c);
        return;
}

void int2str(int i , char*s){
        sprintf(s , "%d" , i);
}
