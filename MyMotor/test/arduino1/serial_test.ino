double vcmd1;

void setup(){
    Serial.begin(9600);
}

void serial_update(){
    static String inString = "";
    if(Serial.available()){
        delay(10);
        char inChar = Serial.read();
        //Serial.write(inChar);
        if (inChar != '\n')
            inString += (char)inChar;
        else {
            vcmd1 = inString.toDouble();
            char result[5];
            inString.toCharArray(result,5);
            Serial.write(result);
            Serial.write('\n');
            inString = ""; // clear the string for new input
        }
    }
}

void loop(){
    serial_update();
    Serial.write("a\n");
    /*if(Serial.available()){
        delay(10);
        char inChar = Serial.read();
        Serial.write(inChar);
    }*/
}




