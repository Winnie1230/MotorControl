#include<stdio.h>
#include<string.h>

void int2str(int , char*);
void result(int , int , char*);

int main(){
	char send[20];
	result(10 ,20 ,send);
	printf("%s\n" , send);
}

void result(int pwm1 , int pwm2 , char s[20]){
	char pwm1_c[10];
	char pwm2_c[10];
	char gpio1[] = "0";
	char gpio2[] = "1";
	//char ans[20];
	int2str(pwm1 , pwm1_c);
	if(strlen(pwm1_c) == 2){
		pwm1_c[3] = '\0';
		pwm1_c[2] = pwm1_c[1];
		pwm1_c[1] = pwm1_c[0];
		pwm1_c[0] = '0';
	}
	int2str(pwm2 , pwm2_c);
	if(strlen(pwm2_c) == 2){
		pwm2_c[3] = '\0';
		pwm2_c[2] = pwm2_c[1];
		pwm2_c[1] = pwm2_c[0];
		pwm2_c[0] = '0';
	}

	strcpy(s , gpio1);
	strcat(s , pwm1_c);
	strcat(s , gpio2);
	strcat(s , pwm2_c);
	//puts(s);
	//puts(pwm2_c);
}

void int2str(int i , char*s){
	sprintf(s , "%d" , i);
}
