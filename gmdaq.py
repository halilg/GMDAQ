#!/usr/bin/env python
import serial,os,sys,datetime,time

ser = serial.Serial('/dev/ttyUSB0', 9600)

while True:
    try:
    	data = ser.readline() 
    except KeyboardInterrupt:
    	ser.close()
    	sys.exit()
    if len(data) > 0:
        print datetime.datetime.now().strftime("%Y%m%dT%H%M%S"), time.time()
        sys.stdout.flush()
        data=""
        
ser.close()


