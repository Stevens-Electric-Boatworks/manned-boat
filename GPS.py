# Sixfab - Reading GPS data with Python
# 2020


from time import sleep
import serial

portwrite = "/dev/ttyUSB2"
port = "/dev/ttyUSB1"
delay = 0.5
print(f"Connecting to port {portwrite}")
try:
    serw = serial.Serial(portwrite, baudrate = 115200, timeout = 1,rtscts=True, dsrdtr=True)
    serw.write('AT$GPSRST\r'.encode())
    sleep(delay)
    serw.write('AT$GPSNVRAM=15,0\r'.encode())
    sleep(delay)
    serw.write('AT$GPSACP\r'.encode())
    sleep(delay)
    serw.write('AT$GPSNMUN=2,1,1,1,1,1,1\r'.encode())
    sleep(delay)
    serw.write('AT$GPSP=1\r'.encode())
    sleep(delay)
    serw.close()
    print("Finished configuration of the GPS")
except Exception as e: 
    print("Serial port connection failed.")
    print(e)
