#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rs232.h"
#include <string.h>

void int2str(int , char*);
void result(int , int , char[]);

int BUF_SIZE = 128;
char send[20];

int main()
{
  int cport_nr = 24; /* /dev/ttyUSB0 */
  int bdrate= 115200; /* 9600 baud */

	int pwm1 , pwm2;
  char mode[]={'8','N','1',0}; // 8 data bits, no parity, 1 stop bit
	//result(70 , 70 , send);
  //char str_send[2][BUF_SIZE]; // send data buffer
  unsigned char str_recv[BUF_SIZE]; // recv data buffer
  //strcpy(str_send[0], "This is a test string.");
  //strcpy(str_send[1], "This is another test string.");

  if(RS232_OpenComport(cport_nr, bdrate, mode))
  {
    printf("Can not open comport\n");
    return(0);
  }
	usleep(20000);

while(1){
	printf("pwm1 pwm2 : ");
	scanf("%d %d" , &pwm1 , &pwm2);
	result(pwm1 , pwm2 , send);
  //usleep(2000000);  /* waits 2000ms for stable condition */
    	RS232_cputs(cport_nr, send); // sends string on serial
	printf("Sent to Arduino: '%s'\n", send);
	usleep(1000000);  /* waits for reply 1000ms */
	int n = RS232_PollComport(cport_nr, str_recv, (int)BUF_SIZE);
	if(n > 0){
      str_recv[n] = 0;   /* always put a "null" at the end of a string! */
      printf("Received %i bytes: '%s'\n", n, (char *)str_recv);
	}
    usleep(5000000);  /* sleep for 5 Second */

}
  return(0);
}

void result(int pwm1 , int pwm2 , char s[20]){
	char pwm1_c[10], pwm2_c[10];
	int2str(pwm1 , pwm1_c);
	 if(strlen(pwm1_c) == 1){
                pwm1_c[3] = '\0';
                pwm1_c[2] = pwm1_c[0];
                pwm1_c[1] = '0';
                pwm1_c[0] = '0';
		printf("1 %s\n" , pwm1_c);
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
	printf("%s\n" , s);
	//printf("%s %d\n" , ans , strlen(ans));
	//printf("re = %s\n" , re);
	return;
}

void int2str(int i , char*s){
	sprintf(s , "%d" , i);
}
