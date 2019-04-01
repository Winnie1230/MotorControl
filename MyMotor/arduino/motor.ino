#include "Mymotor.h"

void setup() {
		pinMode(GPIO1, OUTPUT);
		pinMode(MOTOR1, OUTPUT);
		pinMode(GPIO2, OUTPUT);
		pinMode(MOTOR2, OUTPUT);
		pinMode(ENCODER1,INPUT);
		pinMode(ENCODER2,INPUT);

		EncoderInit();//encoder
		Serial.begin(9600);

		digitalWrite(GPIO1 , LOW);
		digitalWrite(GPIO2 , HIGH);
}

void loop()
{  
		static double input;
		static String inString = "";    // string to hold input
		while (Serial.available() > 0) {
				int inChar = Serial.read();
				if (inChar != '\n')
						inString += (char)inChar;
				else {
						input = inString.toDouble();
						Serial.println(input);
						inString = ""; // clear the string for new input
				}
				CheckInput(input);
		}

		analogWrite(MOTOR1 , PWM1);
		analogWrite(MOTOR2 , PWM2);
		//Serial.print("speed1:");
		//Serial.println(speed1);
		delay(100);
}

void CheckInput(double input){
		if(input < 0){

		}
}
