#define GPIO1 8
#define MOTOR1 9
#define GPIO2 12
#define MOTOR2 5
#define ENCODER1 2
#define ENCODER2 4

#define MAX 200
//const int reso = 9;
const double rad = 0.3490658504;//一格
static volatile double speed1 = 0 , speed2 = 0;
static double vcmd1 = 0 , vcmd2 = 0;

//const double KP_1 = 20 , KI_1 = 30 , KD_1 = 0;
//const double KP_2 = 22 , KI_2 = 30 , KD_2 = 0 ;
volatile int PWM1 = 1 , PWM2 = 0;

/*-----velocity & angle-----*/
static double velocity = 0,angle = 0;


void setup(){
    pinMode(GPIO1, OUTPUT);
    pinMode(MOTOR1, OUTPUT);
    pinMode(GPIO2, OUTPUT);
    pinMode(MOTOR2, OUTPUT);
    pinMode(ENCODER1 , INPUT);
    pinMode(ENCODER2 , INPUT);
    Serial.begin(9600);
    EncoderInit();

    digitalWrite(GPIO1 , HIGH);
    digitalWrite(GPIO2 , LOW);
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
    static double in_1 = 0 , in_2 = 0;
    const double KP_1 = 20 , KI_1 = 30;
    const double KP_2 = 22 , KI_2 = 30;

    static volatile byte encoder_pre1 = 0, encoder_current1 = 0;
    static volatile byte encoder_pre2 = 0, encoder_current2 = 0;
    static volatile byte counter_diff1, counter_diff2;
    static volatile byte counter = 0;

    //static只會被初始化一次 volatile->因為interrupt要使用
    encoder_current1 = digitalRead(ENCODER1);
    encoder_current2 = digitalRead(ENCODER2);

    if(counter >= 100){//頻率10
        //vcmd2 = vcmd1;
        speed1 = (counter_diff1 * rad) * 0.0675 / 0.1 / 20;
        PWM1 = pi_control(KP_1,KI_1,&in_1,vcmd1,speed1);
        speed2 = (counter_diff2 * rad) * 0.0675 / 0.1 / 20;
        PWM2 = pi_control(KP_2,KI_2,&in_2,vcmd2,speed2);
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
    serial_update();

    //String trans = String(speed2 , 2);
    //char s[10] = {};
    //trans.toCharArray(s,10);
    //s[strlen(s)]='\n';
    //Serial.write(s);
    //delay(1000);

    //PWM2 = 100;
    //Serial.write(speed1);
    //char s[10];
    //sprintf(s , "%lf" , speed1);
    //s[strlen(s)]= '\n';
    //Serial.write(s);

    analogWrite(MOTOR1 , PWM1);
    analogWrite(MOTOR2 , PWM2);
}

int pi_control(double KP , double KI , double *in , double vcmd , double sp){
    static int PWM = 0;
    static double G,T;

    ERR = vcmd - sp;
    G = ERR * KP;
    *in = *in + ERR * 0.1 * (KI-T);

    if(G > 255)
        G = 255;
    else if(G < -255)
        G = -255;

    if(*in > 255)
        *in = 255;
    else if(*in < -255)
        *in = -255;

    PWM = (G+*in)*10;
    if(PWM < MAX){
        
    }

    return PWM;

}

void serial_update(){
    static String inString = "";
    if(Serial.available()){
        delay(10);
        char inChar = Serial.read();
        if(inChar != ' ' && inChar != '\n'){
            inString += (char)inChar;
        }
        else{
            if(inChar == ' '){
                velocity = inString.toDouble();
                //char result[10];
                //inString.toCharArray(result,10);
                //sprintf(result,"%lf",velocity);
                //result[strlen(result)]='\n';
                //Serial.write(result);
                inString = "";
            }
            else if(inChar == '\n'){
                angle = inString.toDouble();
                //char result[10];
                //inString.toCharArray(result,10);
                //sprintf(result,"%lf",angle);
                //result[strlen(result)]='\n';
                //Serial.write(result);
                /*if(angle < 0){
                    Serial.write('h');
                    Serial.write('\n');
                }*/
                inString = "";
            }
        }
    }

    if(angle < 0){
        //digitalWrite(GPIO1,LOW);
        //digitalWrite(GPIO2,LOW);
        vcmd1 = 0;
        vcmd2 = abs(angle);
    }
    else if(angle > 0){
        //digitalWrite(GPIO1,HIGH);
        //digitalWrite(GPIO2,HIGH);
        vcmd1 = angle;
        vcmd2 = 0;
    }
    else if(angle == 0){
        //digitalWrite(GPIO1,HIGH);
        //digitalWrite(GPIO2,LOW);
        vcmd1 = velocity;
        vcmd2 = velocity;
    }
}