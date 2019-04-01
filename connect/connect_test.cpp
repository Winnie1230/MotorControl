#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <typeinfo>
#include <wiringPi.h>
#include <sys/time.h>
#include <stdint.h>
#include <math.h>
#include "rs232.h"

using namespace std;

#define ENCODER_PIN1 2 //pin13
#define ENCODER_PIN2 0 //pin11

int reso , sample_time;
double rad ;

//encoder
int encoder_pre;
int encoder_count;
int encoder_check; 
int diff;
int check_num; //read three 1 or 1 then start/num counting
int checktime; //encoder maximum time
int step;
struct timeval start, finish;

//speed
double vcmd1 , vcmd2;
double duration;

//PI control
double KP_1 , KI_1 , KP_2 , KI_2 , G_1 , G_2 , in_1 , in_2;

//send to arduino
int cport_nr = 24;
int bdrate = 115200;
int BUF_SIZE = 20;
char send2[20];

//functions
void encoder(int);
double car_speed(int);
int pi_control(int , double , double); //change speed
//send to arduino
void int2str(int , char*);
void result(int , int , char[]);

//motor_control
void motor_stop();
void motor_acc();
void motor_turn_left();
void motor_turn_right();

void init(){
	reso = 9;
	step = 9;
	rad = 360/(reso * 2) * M_PI / 180 * step;
	sample_time = 1000;

	//encoder
	encoder_pre = 0;
	encoder_count = 0;
	encoder_check = 0;
	diff = -1;
	check_num = 2;
	checktime = 2;

	//speed
	vcmd1 = 0;	vcmd2 = 0;
	duration = 0;
	
	//PI control
	KP_1 = 10;	KP_2 = 16.667;
	KI_1 = 10;	KI_2 = 16.667;
	G_1 = 0;	G_2 = 0; 
	in_1 = 0;	in_2 = 0;
}


int createTCPsocket(int port) // create socket
{
    char inputBuffer[256] = {};
    char message[] = {"Connection success."};
    int sockfd = 0, forClientSockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
        printf("Fail to create a socket.\n");

    // socket connetion
    struct sockaddr_in serverInfo, clientInfo;
    bzero(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(7878);
    bind(sockfd, (struct sockaddr *)&serverInfo, sizeof(serverInfo));
    listen(sockfd, 5);

    socklen_t addrlen = sizeof(clientInfo); // in c++11 can't use int 
                                            // instead of using socklen_t
    forClientSockfd = accept(sockfd, (struct sockaddr *) &clientInfo, &addrlen);
    recv(forClientSockfd, inputBuffer, sizeof(inputBuffer), 0);
    printf("Get:%s\n", inputBuffer);
    send(forClientSockfd, message, sizeof(message), 0);
    return forClientSockfd;
}


void check(string instruction) // check instruction funtion
{
    // ---------- instruction split ----------
    char str[1024];
    strcpy(str, instruction.c_str());
    char * pch;
    std::vector<char *> v;
    pch = strtok (str," ");
    while (pch != NULL)
    {
        printf ("%s\n",pch);
        v.push_back(pch);
        pch = strtok (NULL, " ,.-");
    }
    char* action = v[0];
    char* level = v[1];
    printf("action:%s level:%s\n", action, level);
    // ---------- instruction split ----------
    switch(atoi(action)){ // convert action to 'int' and do decision
        case 0: // stop
            cout << "stop\n" << endl;
            motor_stop();
            break;
        case 1: // velcity modify
            cout << "velcity modify\n" << endl;
	    motor_acc();
            //motor_acc(atoi(level));
            break;
        case 2: // turn left
            cout << "turn left\n" << endl;
	    motor_turn_left();
            //motor_turn_left(atoi(level));
            break;
        case 3: // turn right
            cout << "turn right\n" << endl;
	    motor_turn_right();
            //motor_turn_right(atoi(level));
            break;
        default:
            cout << "Unexcept value\n" << endl;
    }
}


int main()
{
	//char inputBuffer[256] = {};
	//int sockfd_client = createTCPsocket(7878); // create socket at port:7878
	int PWM1 = 0; //initial
	int PWM2 = 0; //initial
	double speed1 , speed2;
	
	//setup
	wiringPiSetup();
	init();
	pinMode(ENCODER_PIN1 , INPUT);
	pinMode(ENCODER_PIN2 , INPUT);
	char mode[] = {'8' , 'N' , '1' , '0'};
	unsigned char str_recv[BUF_SIZE];

	//set connection with arduino
	if(RS232_OpenComport(cport_nr , bdrate , mode)){
		printf("fuck\n");
		return(0);
	}
	usleep(20000);

	printf("start\n");

	while(true){
	//int inst_length = read(sockfd_client, inputBuffer, sizeof(inputBuffer)); // receive instruction
	/*if(inst_length > 0) // receive something
        {
            printf("Get:%s\n", inputBuffer);
            check(inputBuffer);
            memset(inputBuffer, 0, sizeof(inputBuffer)); // clear inputBuffer
        }
        else // receive nothing
            printf("hey hey\n");
    */    
	//send PWM1 PWM2 to arduino
	result(PWM1 , PWM2 , send2); //send2 string, control_sig char*
	RS232_cputs(cport_nr , send2); //send string on serial
	usleep(500000);
	int n = RS232_PollComport(cport_nr , str_recv , (int)BUF_SIZE);
	if(n > 0){
		str_recv[n] = 0;
		printf("Received %i bytes: '%s'\n" , n , (char*)str_recv);
		RS232_flushRXTX(cport_nr);
		usleep(100000);
	}
        
        speed1 = car_speed(1);
        pi_control(1 , vcmd1 , speed1);
        speed2 = car_speed(2);
        pi_control(2 , vcmd2 , speed2);
        //usleep(100000); // delay
	printf("%lf %d %lf %d\n" , speed1 , PWM1 , speed2 , PWM2);
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

double car_speed(int pin){
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

        //printf("\nduration = %lf\n" , duration);
        double speed = rad * 0.0675 / duration;
        encoder_count = 0;
        return speed;
}

int pi_control(int num , double vcmd, double speed){
        int PWM = 0;
        double ERR , G , in;

        ERR = vcmd - speed;
        //printf("ERR = %lf\n" , ERR);

        if(num == 1){
                G_1 = ERR * KP_1;
                in_1 = in_1 + ERR * duration * KI_1;
                
                G = G_1;
                in = in_1;
        }
        else{
                G_2 = ERR * KP_2;
                in_2 = in_2 + ERR * duration * KI_2;
                
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
	strcpy(s , gpio1);
	strcat(s , pwm1_c);
	strcat(s , gpio2);
	strcat(s , pwm2_c);
        return;
}

void int2str(int i , char*s){
        sprintf(s , "%d" , i);
}

void motor_stop(){
	vcmd1 = 0;
	vcmd2 = 0;
}

void motor_acc(){
	vcmd1 = 1;
	vcmd2 = 1;
	/*switch(level){
		case 1:
		vcmd1 = 0.5;
		vcmd2 = 0.5;
		break;
        
		case 2:
		vcmd1 = 1;
		vcmd2 = 1;
		break;
	
		case 3:
		vcmd1 = 1.5;
		vcmd2 = 1.5;
		break;
        
		case 4:
		vcmd1 = 2;
		vcmd2 = 2;
		break;
        
        	case 5:
		vcmd1 = 2.5;
		vcmd2 = 2.5;
		break;
    }*/
}

void motor_turn_left(){
    /*switch(level){
        case 1:
	break;
    }*/
	vcmd1 = 0.3;
	vcmd2 = 0.6;
}

void motor_turn_right(){
    /*switch(level){
        case 1:
            break;
    }*/
	vcmd1 = 0.6;
	vcmd2 = 0.3;
}
