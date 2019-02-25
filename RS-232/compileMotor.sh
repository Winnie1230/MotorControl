g++ -pthread  motor.cpp rs232.c -Wno-write-strings -lwiringPi -lm -lrt -lcrypt -o motor
sudo ./motor

