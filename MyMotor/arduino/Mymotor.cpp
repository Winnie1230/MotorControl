#include"Mymotor.h"

//const int reso = 9;
//const double rad = 0.3490658504;//一格
char mode;
volatile double vcmd1 = 0.5 ,vcmd2 = 0;
volatile double speed1 = 0 , speed2 = 0;

//-----pi control-----
const double KP_1 = 200 , KI_1 = 0;
const double KP_2 = 180 , KI_2 = 90;
volatile int PWM1 = 0 , PWM2 = 0;

void EncoderInit(){
		TCCR2A = 0;
		TCCR2B = 0; 
		TCCR2B |= (1<<WGM22);
		TCCR2B |= (1<<CS22) | (1<<CS20);
		TIMSK2 |= (1 << OCIE2A);
		TCNT2=0;
		OCR2A = 14; //頻率1000
}

void EncoderDisable(){
		TCCR2A = 0;
		TCCR2B = 0; 
}

ISR(TIMER2_COMPA_vect){
		static volatile byte encoder_pre1 = 0, encoder_current1 = 0;
		static volatile byte encoder_pre2 = 0, encoder_current2 = 0;
		static volatile byte counter_diff1, counter_diff2;
		static volatile byte counter = 0;

		//static只會被初始化一次 volatile->因為interrupt要使用
		encoder_current1 = digitalRead(ENCODER1);
		encoder_current2 = digitalRead(ENCODER2);

		if(counter >= 100){//頻率10
				speed1 = (counter_diff1 * rad) * 0.0675 / 0.1 / 20;
				PWM1 = PiControl(1,vcmd1,speed1);
				//speed2 = (counter_diff2 * rad) * 0.0675 / 0.1 / 20;
				//PWM2 = PiControl(2,vcmd2,speed2);
				counter_diff1 = 0; counter_diff2 = 0;
				counter = 0; 
		}

		if(encoder_current1 != encoder_pre1)
				counter_diff1 ++;
		if(encoder_current2 != encoder_pre2)
				counter_diff2 ++;

		counter++;
		encoder_pre1 = encoder_current1;
		encoder_pre2 = encoder_current2;
}

int PiControl(int num , double vcmd , double sp){
		static double ERR = 0 , G = 0 , in = 0;
		static int PWM = 0;
		static double G_1 , G_2 , in_1 , in_2;

		ERR = vcmd - sp;
		if(num == 1){//motor1
				G_1 = ERR * KP_1;
				in_1 = in_1 + ERR * 0.1 * KI_1;
				G = G_1; in = in_1;
		}
		else{//motor2
				G_2 = ERR * KP_2;
				in_2 = in_2 + ERR * 0.1 * KI_2;
				G = G_2; in = in_2;
		}
		if(G > 255)
				G = 255;
		else if (G < -255)
				G = -255;

		if(in > 255)
				in = 255;
		else if(in < -255)
				in = -255;

		PWM = G + in;
		if(PWM <= 0)
			PWM = 0;
		else
			PWM >= 200 ? PWM = 200 : PWM = PWM;

		return PWM;
}
