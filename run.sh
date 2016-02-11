#!/bin/sh
gcc -pthread -o m1 motor_control_single.c -lwiringPi -lm -lrt -lcrypt
gcc -pthread -o m2 motor_control_single2.c -lwiringPi -lm -lrt -lcrypt
nohup sudo ./m1 > o1.o &
nohup sudo ./m2 > o2.o

