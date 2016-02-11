#!/bin/sh
gcc -pthread -o motor_control motor_control.c -lwiringPi -lm -lrt -lcrypt
sudo ./motor_control
