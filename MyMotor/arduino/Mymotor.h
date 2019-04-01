#ifndef _MY_MOTOR
#define _MY_MOTOR

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define GPIO1 8
#define MOTOR1 9
#define GPIO2 12
#define MOTOR2 5
#define ENCODER1 2
#define ENCODER2 3

#define rad 0.3490658504
extern char mode;

// ------pi control------
extern const double KP_1 , KI_1 , KP_2 , KI_2;

// -----pcmd-----
extern volatile double vcmd1,vcmd2;
extern volatile int PWM1 , PWM2;
extern volatile double speed1 , speed2;

void EncoderInit(); //interrupt
void EncoderDisable();
int PiControl(int,double,double);
void ChangeVcmd(char);


#endif
