gcc -pthread  arduino_test_1.c rs232.c -Wno-write-strings -lwiringPi -lm -lrt -lcrypt -o arduino_test_1
sudo ./arduino_test_1
