import time
import math
import RPi.GPIO as GPIO

encoder_pin = 2
motor_control_pin_1 = 18
motor_control_pin_2 = 12

GPIO.setmode(GPIO.BCM)
GPIO.setup(encoder_pin,GPIO.IN) #encoder signal
GPIO.setup(motor_control_pin_1,GPIO.OUT) #motor control
GPIO.setup(motor_control_pin_2,GPIO.OUT)

reso = 20 #resolution black/white number
rad = math.radians(360/(reso*2))
sample_time = 0.0001

encoder_pre = 0
encoder_count = 0
encoder_check = 0
sum_c = 0

def encoder():
	global encoder_pre, encoder_count, encoder_check
	encoder_current = GPIO.input(encoder_pin)
	if encoder_current != encoder_pre:
		if encoder_check == 0:
			encoder_check = 1 #start to count
		else:				
			encoder_check = 0 #end count
	elif encoder_current == encoder_pre and encoder_check == 1:
		encoder_count += 1
	else:
		pass

def car_speed():
	global encoder_count, encoder_check, sum_c
	while True:
		encoder()
		time.sleep(sample_time)
		sum_c += 1
		if encoder_count > 0 and encoder_check == 0:
			break
#	print(encoder_count)
	speed = rad * 0.013 /(encoder_count * sample_time)
	encoder_count = 0
#	print("speed =",speed)

def motor_stop():
	GPIO.output(motor_control_pin_1,GPIO.HIGH)
	GPIO.output(motor_control_pin_2,GPIO.HIGH)

if __name__ == '__main__':
	motor_stop()
	while True:
		try:
			if sum_c == 0:
				pre_time = time.time()
			GPIO.output(motor_control_pin_1,GPIO.HIGH)
			GPIO.output(motor_control_pin_2,GPIO.LOW)		
			car_speed()
			if sum_c >= 100:
				current_time = time.time()
				sum_c = 0
				print(current_time - pre_time)
		except KeyboardInterrupt:
			print("Program close")
			GPIO.cleanup()
