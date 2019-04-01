#!/bin/sh
g++ -pthread modify_motor.cpp rs232.c -Wno-write-strings -lwiringPi -lm -lrt -lcrypt -o modify_motor
./modify_motor
