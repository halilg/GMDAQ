#!/usr/bin/env python
# Data acquisition daemon
# Receives data from serial ports, time stamps and streams to named pipes.

import serial,os,sys,datetime,time,uuid
import logging, logging.handlers
from serial_ports import serial_ports
import threading


import cPickle
class trivialDB():
    def __init__(self, dbname):
        self.__db=None
        self.__dbname=dbname
        self.__data={}
    
    def get(self, key):
        self.read()
        return self.__data[key]

    def put(self, key, value):
        self.__data[key] = value

    def read(self):
        try:
            self.__db = open(self.__dbname, 'rb')
            self.__data = cPickle.load(self.__db)
            self.__db.close()
        except IOError:
            self.write()

    def write(self):
        self.__db = open(self.__dbname, 'wb')
        cPickle.dump(self.__data, self.__db)
        self.__db.close()

SBAUD=115200

# Serial communication class with protocol support
class GMSERP(threading.Thread):
    portOpen=False
    def __init__(self, logger):
        self.__logger = logger
        self.__transmitter = None
        self.isGM = False
        self.isEnv = False
        self.workdir=os.path.dirname(logger.handlers[0].baseFilename)
        threading.Thread.__init__(self)
            
    def connect(self, port='/dev/ttyUSB0', baud=SBAUD):
        try:
            self.__logger.debug("Opening serial port: %s" % port)
            self.__ser = serial.Serial(port, baud)
        except OSError:
            self.__logger.error("Serial port couldn't be opened: %s" % (port))
            portOpen=False
            return
        self.portOpen=self.__ser.isOpen()
        if self.portOpen:
            self.__logger.debug('Serial port opened: %s' %(port))
        else: 
            self.__logger.error('Serial port open timeout')

        # When we opened the serial port, Arduino resets itself. Let's check if it says something:
        while True:
            try:
                data = self.__ser.readline()
            except KeyboardInterrupt:
                self.__ser.close()
                self.__logger.info("Serial port closed")
                return
            if len(data) > 0:
                break
                
        self.__logger.debug("Received: %s" % data.strip())
        data = data.strip()
        if data == "!IAM AUNO": #can't we do this with a dictionary instead?
            self.isGM = True
            self.__desc = "GM module"
            self.__transmitter = transmitter(self.__logger,NPGM)
        elif data == "!IAM ADUE":
            self.isEnv = True
            self.__desc = "Environment sensors"
            self.__transmitter = transmitter(self.__logger,NPENV)
        
    #def register_cb(self, fncref):
    #    self.__callback=fncref
        
    def __write_data(self, data):
        secs = time.time()*1000
        data = "%12d  %s" % (secs, data.strip())
        self.__transmitter.send(data)

    def stop(self):
        self._Thread__stop()
        self.__ser.close()
        self.__transmitter.close()
        self.__logger.info("Disconnected (%s)" % self.__desc)
        
    def run(self):
       # try: 
            while True:
                data = self.__ser.readline() 
                if len(data) > 0:
                    self.__write_data(data)
                    data=""
       # except SystemExit:
       #     self.__ser.close()
       #     self.__logger.info("Serial port closed")
       #     return

def receive_data_GM(data):
    secs = time.time()*1000
    data = "%12d  %s" % (secs, data.strip())
    print data

def receive_data_env(data):
    secs = time.time()*1000
    data = "%12d  %s" % (secs, data.strip())
    print data

if __name__ == '__main__':
    logger = logging.getLogger('gmdaq')
    rfh = logging.handlers.RotatingFileHandler('GMDAQ.log',
                                               maxBytes=100000,
                                               backupCount=100,)
    
    isDaemon = False
    if len(sys.argv) > 1:
        if sys.argv[1] == "-d": isDaemon = True
    
    formatter = logging.Formatter('%(asctime)s : %(levelname)s - %(message)s')
    rfh.setFormatter(formatter)
    logger.addHandler(rfh)
    #logger.setLevel(logging.INFO)
    logger.setLevel(logging.DEBUG)
    logger.info("datalogd starting.")
    serials = [GMSERP(logger), GMSERP(logger)]
    connGM = None # Serial connection with GM readout 
    connEnv = None # Serial connection with environment sensors readout
    
    persistentdata=trivialDB("datalogd.dat")
    pidfname=os.path.join('/Users/halil/work/GMDAQ','datackd.pid')
    runNumber=0
    try: runNumber = persistentdata.get("runNumber")
    except KeyError:
        pass
    
    runNumber += 1
    persistentdata.put("runNumber",runNumber)
    persistentdata.write()    
    
    serialports=serial_ports()
    
    logger.debug("Available serial ports: %s" % serialports)
    i=0
    for port in serialports:
        if "Bluetooth" in port: continue
        serials[i].connect(port)
        if serials[i].portOpen:
            if serials[i].isGM : connGM = serials[i]
            if serials[i].isEnv : connEnv = serials[i]
            i=i+1
            

    if connGM:
        logger.info("Connected to GM module")
 #       connGM.register_cb(receive_data_GM)
        connGM.setDaemon(True)
        connGM.start()
        
    if connEnv:
        logger.info("Connected to environment sensors")
#        connEnv.register_cb(receive_data_env)
        connEnv.setDaemon(True)
        connEnv.start()
    
    try:
        while True:
            time.sleep(5)
            if os.path.exists("./datalogd.py.quit"):
                logger.debug("Exiting (File exists: ./datalogd.py.quit)")
                os.remove("./datalogd.py.quit")
                connGM.stop()
                connEnv.stop()
                sys.exit()
    except KeyboardInterrupt:
        logger.debug("Exiting (KeyboardInterrupt)")
        connGM.stop()
        connEnv.stop()
        sys.exit()
