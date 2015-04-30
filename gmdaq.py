#!/usr/bin/env python
import serial,os,sys,datetime,time,uuid
import logging, logging.handlers

serialports=["/dev/cu.usbmodem1471","/dev/cu.usbmodem801211", "/dev/cu.usbmodem1411", "/dev/ttyUSB0"]
runstart=0.0

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
    
    

class GMSER():
    portOpen=False
    def __init__(self, logger):
        self.__logger = logger
            
    def connect(self, port='/dev/ttyUSB0', baud=115200):
        try:
            self.__ser = serial.Serial(port, baud)
        except OSError:
            self.__logger.error("Serial port couldn't be opened: %s" % (port))
            portOpen=False
            return
        self.portOpen=self.__ser.isOpen()
        if self.portOpen:
            self.__logger.info('Serial port opened: %s' %(port))
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
    #__maxuuids=1000
    #__uuds=[]
    __counts_per_file=12000
    __counts=0
    __f=None
    __logfile = None
    __ofnum=0
    portOpen = False
    
    def __init__(self, runNumber, logger):
        self.__logger = logger
        self.__runNumber = runNumber
        self.__DAQ=GMSER(self.__logger)
        
        for port in serialports:
            self.__DAQ.connect(port)
            self.portOpen = self.__DAQ.portOpen
            if self.portOpen: break
        if not self.portOpen:
            return
        
        # Create the data directory if it doesn't exist
        if not (os.path.exists(self.__datadir) and os.path.isdir(self.__datadir) ):
            try: os.mkdir (self.__datadir)
            except:
                self.__logger.error( ("Failed to create directory: %s") % self.__datadir)
                return

        self.__DAQ.register_cb(self.__record_data)
        #self.__uuds=getuuids(self.__maxuuids)
        
    def __record_data(self, data):
        self.__counts += 1
        if self.__counts % self.__counts_per_file == 0:
            self.__of = self.__getNewDataFile()
            self.__logger.info( ("%d hits, writing to new data file: %s") % (self.__counts, self.__ofname) )
        self.__of.write(datetime.datetime.now().strftime("%Y%m%dT%H%M%S   "))
        secs=time.time()*1000
        print secs,"%12d   " % secs
        self.__of.write("%12d   " % secs )
        self.__of.write(data)

    def __getNewDataFile(self):
        self.__ofnum += 1
        self.__ofname=self.__datafnprefix+str("%05d" % self.__runNumber)+("_%02d" % self.__ofnum)+self.__datafnpostfix
        self.__ofname=os.path.join(self.__datadir,self.__ofname)
        return file(self.__ofname,'w')

    def setLogger(logger):
        self.__log=logger

    def start(self):
        self.__of=self.__getNewDataFile()
        self.__logger.info("Starting data taking")
        self.__logger.info( ("Writing data to: %s") % self.__ofname)
        self.__DAQ.run_forever()
        

if __name__ == '__main__':
    #logging.basicConfig(
    #    format='%(levelname)s:%(asctime)s %(message)s',
    #    level=logging.DEBUG)
    persistentdata=trivialDB("gmdaq.pkl")
    runNumber=0
    try: runNumber = persistentdata.get("runNumber")
    except KeyError:
        pass
    
    runNumber += 1
    persistentdata.put("runNumber",runNumber)
    persistentdata.write()

    logger = logging.getLogger('gmdaq')
    rfh = logging.handlers.RotatingFileHandler('GMDAQ.log',
                                               maxBytes=100000,
                                               backupCount=100,
                                               )
    #fh = logging.FileHandler()
    formatter = logging.Formatter('%(asctime)s : %(levelname)s - %(message)s')
    rfh.setFormatter(formatter)
    logger.addHandler(rfh)
    logger.setLevel(logging.INFO)
    logger.info("gmdaq starting. Run #%05d" % runNumber)
    mydaq=GMDAQ(runNumber, logger)
    if not mydaq.portOpen:
        logger.error("To list available serial ports: python -m serial.tools.list_ports")
        sys.exit()
    try:
        mydaq.start()
    except KeyboardInterrupt:
        pass
    rfh.close()
    


