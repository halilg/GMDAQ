#!/usr/bin/env python
import serial,os,sys,datetime,time,ipc
import logging, logging.handlers    

class GMSER:
    portOpen=False
    def __init__(self, logger):
        self.__logger = logger
            
    def connect(self, port='/dev/ttyUSB0', baud=115200):
        try:
            self.__ser = serial.Serial(port, baud)
        except OSError:
            self.__logger.error("Serial port couldn't be opened")
            portOpen=False
            return
        self.portOpen=self.__ser.isOpen()
        if self.portOpen:
            self.__logger.info('Serial port opened')
        else:
            self.__logger.info('Serial port open timeout')
        
    def stop(self):
        self.__ser.close()
        
    def run_forever(self):
        while True:
            try:
                data = self.__ser.readline() 
            except KeyboardInterrupt:
                self.__ser.close()
                return
            if len(data) > 0: # received data from serial port
                timestamp=datetime.datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
                opipe.write(timestamp+data+'\n') !!!
                data=""
        
if __name__ == '__main__':
    #logging.basicConfig(
    #    format='%(levelname)s:%(asctime)s %(message)s',
    #    level=logging.DEBUG)
    logger = logging.getLogger('gmdaq')
    rfh = logging.handlers.RotatingFileHandler('stamper.log',
                                               maxBytes=1000,
                                               backupCount=100,
                                               )
    #fh = logging.FileHandler()
    formatter = logging.Formatter('%(asctime)s : %(levelname)s - %(message)s')
    rfh.setFormatter(formatter)
    logger.addHandler(rfh)
    logger.setLevel(logging.INFO)
    logger.info("gmdaq starting")
    mydaq=GMSER(logger)
    if not mydaq.portOpen:
        logger.error("Couldn't open serial port. Exiting.")
        sys.exit()
    try:
        mydaq.start()
    except KeyboardInterrupt:
        pass
    rfh.close()
    


