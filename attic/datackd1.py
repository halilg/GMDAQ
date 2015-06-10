#!/usr/bin/env python
# Data acquisition daemon
# Receives data from serial ports, time stamps and streams to named pipes.

import serial,os,sys,datetime,time,uuid
import logging, logging.handlers
from serial_ports import serial_ports
from multiprocessing import Process, Pipe

SBAUD=115200

# Serial communication class with protocol support
class GMSERP(Process):
    portOpen=False
    def __init__(self, logger):
        self.__logger = logger
        self.isGM = False
        self.isEnv = False        
        self.workdir=os.path.dirname(logger.handlers[0].baseFilename)
        #threading.Thread.__init__(self)
            
    def connect(self, port='/dev/ttyUSB0', baud=SBAUD):
        print self._parent_pid
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
                
    def __write_data(self, data):
        secs = time.time()*1000
        data = "%12d  %s" % (secs, data.strip())
        print data

    def stop(self):
        self.__ser.close()
        self.__logger.info("Disconnected (%s)" % self.__desc)
        
    def run(self):
       # try: 
            while True:
                data = self.__ser.readline() 
                if len(data) > 0:
                    self.__write_data(data)
                    data=""

if __name__ == '__main__':
    connGM = None # Serial connection with GM readout 
    connEnv = None # Serial connection with environment sensors readout
    logger = logging.getLogger('gmdaq')
    rfh = logging.handlers.RotatingFileHandler('GMDAQ.log',
                                               maxBytes=100000,
                                               backupCount=100,)
    
    #isDaemon = False
    if len(sys.argv) > 1:
        if sys.argv[1] == "-d": isDaemon = True
    
    formatter = logging.Formatter('%(asctime)s : %(levelname)s - %(message)s')
    rfh.setFormatter(formatter)
    logger.addHandler(rfh)
    #logger.setLevel(logging.INFO)
    logger.setLevel(logging.DEBUG)
    logger.info("datalogd starting.")

    pidfname=os.path.join('/Users/halil/work/GMDAQ','datackd.pid')
    serialports=serial_ports()
    
    logger.debug("Available serial ports: %s" % serialports)
    i=0
    s_port_workers=[]
    for port in serialports:
        if "Bluetooth" in port: continue
        myport=GMSERP(logger)
        myport.connect(port)
        if myport.portOpen:
            if myport.isGM : connGM = myport
            if myport.isEnv : connEnv = myport
            s_port_workers.append(myport)
            i=i+1
            
    if connGM:
        logger.info("Connected to GM module")
        #connGM.daemon = True
        connGM.start()
        
    if connEnv:
        logger.info("Connected to environment sensors")
        #connEnv.daemon = True
        connEnv.start()
    
    
    print s_port_workers
    sys.exit()
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
