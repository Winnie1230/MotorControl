#define GPIO1 8
#define MOTOR1 9
#define GPIO2 12
#define MOTOR2 5
#define ENCODER1 2
#define ENCODER2 4

//const int reso = 9;
const double rad = 0.3490658504;//一格
static volatile double speed1 = 0 , speed2 = 0;
static double vcmd1 = 0 , vcmd2 = 0;

const double KP_1 = 20 , KI_1 = 30 , KD_1 = 0;
const double KP_2 = 22 , KI_2 = 30 , KD_2 = 0 ;
volatile int PWM1 = 1 , PWM2 = 0;


void setup(){
    pinMode(GPIO1, OUTPUT);
    pinMode(MOTOR1, OUTPUT);
    pinMode(GPIO2, OUTPUT);
    pinMode(MOTOR2, OUTPUT);
    pinMode(ENCODER1 , INPUT);
    pinMode(ENCODER2 , INPUT);
    Serial.begin(9600);
    EncoderInit();

    digitalWrite(GPIO1 , LOW);
    digitalWrite(GPIO2 , HIGH);
}

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
        vcmd2 = vcmd1;
        speed1 = (counter_diff1 * rad) * 0.0675 / 0.1 / 20;
        PWM1 = pi_control(1,vcmd1,speed1);
        speed2 = (counter_diff2 * rad) * 0.0675 / 0.1 / 20;
        PWM2 = pi_control(2 , vcmd2 , speed2);
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


void loop(){ 
    //static double input;
    //static String inString = "";    // string to hold input
    serial_update();
    /*while (Serial.available() > 0) {
        int inChar = Serial.read();
        if (inChar != '\n')
            inString += (char)inChar;
        else {
            vcmd1 = inString.toDouble();
            Serial.println(inString);
            inString = ""; // clear the string for new input
        }
    }*/
    

    String trans = String(speed2 , 2);
    char s[10] = {};
    trans.toCharArray(s,10);
    s[strlen(s)]='\n';
    Serial.write(s);
    //delay(1000);

    //PWM2 = 100;
    //Serial.write(speed1);
    //char s[10];
    //sprintf(s , "%lf" , speed1);
    //s[strlen(s)]= '\n';
    //Serial.write(s);
    //Serial.print("ERR:");
    //Serial.println(ERR);

    analogWrite(MOTOR1 , PWM1);
    analogWrite(MOTOR2 , PWM2);
}

int pi_control(int num , double vcmd , double sp){
    static int PWM = 0;
    static double G_1 , G_2 , in_1 , in_2 , d_1 , d_2;
    static double ERR = 0 , G = 0 , in = 0 , PRE_ERR_1 = 0 , PRE_ERR_2 = 0 , D = 0;
    
    ERR = vcmd - sp;
    //Serial.print("ERR:");
    //Serial.println(ERR);
    if(num == 1){//motor1
        G_1 = ERR * KP_1;
        in_1 = in_1 + ERR * 0.1 * KI_1;
        d_1 = (ERR - PRE_ERR_1) / 0.1 * KD_1;
        PRE_ERR_1 = ERR;
        G = G_1; in = in_1; D = d_1;
    }
    else{//motor2
        G_2 = ERR * KP_2;
        in_2 = in_2 + ERR * 0.1 * KI_2;
        d_2 = (ERR - PRE_ERR_2) / 0.1 * KD_2;
        PRE_ERR_2 = ERR;
        G = G_2; in = in_2; D = d_2;
    }
    if(G > 255)
        G = 255;
    else if (G < -255)
        G = -255;

    if(in > 255)
        in = 255;
    else if(in < -255)
        in = -255;

    if(D > 255)
        D = 255;
    else if(D < -255)
        D = -255;

    PWM = (G + in + D) * 10;
    if(PWM <= 0)
        PWM = 0;
    else
        abs(PWM) >= 200 ? PWM = 200 : PWM = abs(PWM);
    return PWM;

}

void serial_update(){
    static String inString = "";
    if(Serial.available()){
        delay(10);
        char inChar = Serial.read();
        if (inChar != '\n')
            inString += (char)inChar;
        else {
            vcmd1 = inString.toDouble();
            //char result[10];
            //inString.toCharArray(result,10);
            //sprintf(result , "%lf" , vcmd1);
            //result[strlen(result)]='\n';
            //Serial.write(result);
            inString = ""; // clear the string for new input
        }
    }
}
