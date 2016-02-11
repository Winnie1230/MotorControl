import RPi.GPIO as GPIO
import time
import sys
# trigger_pin = 23
# echo_pin = 24

def set_mode(trigger_pin, echo_pin):
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(trigger_pin, GPIO.OUT)
    GPIO.setup(echo_pin, GPIO.IN)

def send_trigger_pulse(trigger_pin):
	GPIO.output(trigger_pin,True)
	time.sleep(0.01)
	GPIO.output(trigger_pin, False)

def wait_for_echo(value, timeout, echo_pin):
	count=timeout
	while GPIO.input(echo_pin)!= value and count > 0:
		count = count-1

def get_distance(trigger_pin, echo_pin):
	send_trigger_pulse(trigger_pin)
	wait_for_echo(True,50000, echo_pin)
	start = time.time()
	wait_for_echo(False,50000, echo_pin)
	finish = time.time()
	pulse_len = finish-start
	distance_cm = pulse_len*340*100/2
	distance_in = distance_cm/2.5
	return(distance_cm,distance_in)
while True:
	#set_mode(7,8)
	#print("7 8 cm=%f\tinches=%f" % get_distance(7,8), end='\r')
	set_mode(23, 24)
	print("23 24 cm=%f\tinches=%f" % get_distance(23,24), end='\r')
	time.sleep(1)
	sys.stdout.flush()
