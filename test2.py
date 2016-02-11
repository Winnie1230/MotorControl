import RPi.GPIO as GPIO
import time

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

TRIG = 23
ECHO = 24

GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

def get_distance():
    GPIO.output(TRIG, False)
    time.sleep(2)
    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)

    while GPIO.input(ECHO) == 0:
#        print('start', ECHO)
        start = time.time()
    while GPIO.input(ECHO) == 1:
#        print('edn', ECHO)
        end = time.time()
    return (end - start) * 17150

while True:
    print(get_distance(), 'cm')
    time.sleep(1)
GPIO.cleanup()
