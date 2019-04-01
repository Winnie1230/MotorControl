import serial
import time
ser = serial.Serial('/dev/ttyACM0',9600)

if (ser.isOpen()):
    print("connect\n")

    while 1:
        vcmd = input("Enter vcmd:")
        try:
            val = float(vcmd)
            print("Input float number value is:" , val)
            ser.write(vcmd.encode())
            ser.write('\n'.encode())
            #ser.flushOutput()
            time.sleep(0.1)
        except ValueError:
            print("Error")
        try:
            s = ""
            while(ser.in_waiting > 0):
                #print("received")
                line = ser.read()
                line = line.decode(encoding='utf-8', errors='ignore')
                #line = line.decode();
                s += line;
            print(s)
            #ser.flushInput()
            #string1 = "hello\n";
            #string1_encode = string1.encode()
            
            #ser.write(string1_encode)
            #ser.flushOutput()
            #time.sleep(0.1)
        except KeyboardInterrupt:
            ser.close()
            if(~ser.isOpen()):
                print("closed\n")

