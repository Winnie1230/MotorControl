#include <wiringPi.h>
//#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (void)
{
  //int bright ;

  printf ("Raspberry Pi wiringPi PWM test program\n") ;

	if (wiringPiSetup () == -1)
		exit (1) ;

	//softPwmCreate(1, 50 , 100);
	//softPwmCreate(23 , 50, 100);

	pinMode (1 , PWM_OUTPUT);
	//pinMode( 23 ,PWM_OUTPUT);

	while(1){
		//softPwmWrite(1 , 60);
		//softPwmWrite(23 , 10);
		pwmWrite(1 , 900);
		//pwmWrite(23 , 1023);
	}


  /*for (;;)
  {
    for (bright = 0 ; bright < 1024 ; ++bright)
    {
      pwmWrite (1, bright) ;
		pwmWrite(23 , bright);
      delay (1) ;
    }

    for (bright = 1023 ; bright >= 0 ; --bright)
    {
      pwmWrite (1, bright) ;
		pwmWrite(23 , bright);
      delay (1) ;
    }
  }*/

  return 0 ;
}
