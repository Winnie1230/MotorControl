import time
from threading import Timer
def motor_speed():
	print(time.ctime())

if __name__ == '__main__':
	while True:
		t = Timer(10,motor_speed)
		t.start()

