#!/usr/bin/env python
import serial,os,sys,datetime,time,uuid
import logging, logging.handlers


def getuuids(maxuuids):
    uuds=[]
    for i in range(maxuuids):
        uuds.append(str(uuid.uuid4()))
    return uuds

def logevent(lf, message):
    timestamp=datetime.datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
    lf.write(timestamp+message+'\n')

class GMSER():
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
    def register_cb(self, fncref):
        self.__callback=fncref
        
    def stop(self):
        self.__ser.close()
        
    def run_forever(self):
        while True:
            try:
                data = self.__ser.readline() 
            except KeyboardInterrupt:
                self.__ser.close()
                return
            if len(data) > 0:
                self.__callback(data)
                data=""
        

class GMDAQ():
    __ofname=''    
    __datadir='data'
    __datafnprefix='data_'
    __datafnpostfix='.txt'
    __maxuuids=4
    __uuds=[]
    __counts_per_file=1200
    __counts=0
    __f=None
    __logfile = None
    portOpen = False
    
    def __init__(self, logger):
        self.__logger = logger
        self.__DAQ=GMSER(self.__logger)
        
        # Create the data directory is it doesn't exist
        if not (os.path.exists(self.__datadir) and os.path.isdir(self.__datadir) ):
            try: os.mkdir (self.__datadir)
            except:
                self.__logger.error( ("Failed to create directory: %s") % self.__datadir)
                return
        
        # to list serial ports on a Mac: python -m serial.tools.list_ports
        self.__DAQ.connect("/dev/cu.usbmodem801211")
        self.portOpen = self.__DAQ.portOpen
        if not self.portOpen:
            return
        self.__DAQ.register_cb(self.__record_data)
        self.__uuds=getuuids(self.__maxuuids)
        
    def __record_data(self, data):
        self.__counts += 1
        if self.__counts % self.__counts_per_file == 0:
            self.__of = self.__getNewDataFile()
            self.__logger.info( ("%d hits, writing to new data file: %s") % (self.__counts, self.__ofname) )
        self.__of.write(datetime.datetime.now().strftime("%Y%m%dT%H%M%S   "))
        self.__of.write(str(time.time())+'\n')
        #print data,

    def __getNewDataFile(self):
        self.__ofname=self.__datafnprefix+self.__uuds.pop()+self.__datafnpostfix
        self.__ofname=os.path.join(self.__datadir,self.__ofname)
        return file(self.__ofname,'w')

    def setLogger(logger):
        self.__log=logger

    def start(self):
        self.__of=self.__getNewDataFile()
        self.__logger.info("Starting data taking")
        self.__logger.info( ("Writing data to: %s") % self.__ofname)
        print 'Taking data to:',self.__ofname
        self.__DAQ.run_forever()
        

if __name__ == '__main__':
    #logging.basicConfig(
    #    format='%(levelname)s:%(asctime)s %(message)s',
    #    level=logging.DEBUG)
    logger = logging.getLogger('gmdaq')
    rfh = logging.handlers.RotatingFileHandler('GMDAQ.log',
                                               maxBytes=1000,
                                               backupCount=100,
                                               )
    #fh = logging.FileHandler()
    formatter = logging.Formatter('%(asctime)s : %(levelname)s - %(message)s')
    rfh.setFormatter(formatter)
    logger.addHandler(rfh)
    logger.setLevel(logging.INFO)
    logger.info("gmdaq starting")
    mydaq=GMDAQ(logger)
    if not mydaq.portOpen:
        logger.error("Couldn't open serial port. Exiting.")
        sys.exit()
    try:
        mydaq.start()
    except KeyboardInterrupt:
        pass
    rfh.close()
    


