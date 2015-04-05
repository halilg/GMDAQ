#!/usr/bin/env python
import serial,os,sys,datetime,time,uuid

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
    def __init__(self, logfile):
        self.__logfile = logfile
    
    def connect(self, port='/dev/ttyUSB0', baud=9600):
        try:
            self.__ser = serial.Serial(port, baud)
        except OSError:
            logevent(self.__logfile,"ERROR: Serial port couldn't be opened")
            portOpen=False
            return
        self.portOpen=ser.isOpen()
        if self.portOpen:
            logevent(self.__logfile,'Serial port opened')
        else:
            logevent(self.__logfile,'Serial port open timeout')
        
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
    __counts_per_file=10000
    __counts=0
    __f=None
    __logfile = None
    portOpen = False
    
    def __init__(self, logfile):
        self.__logfile = logfile
        self.__DAQ=GMSER(self.__logfile)
        self.__DAQ.connect()
        self.portOpen = self.__DAQ.portOpen
        if not self.portOpen: return
        self.__DAQ.register_cb(self.__record_data)
        self.__uuds=getuuids(self.__maxuuids)
        
    def __record_data(self, data):
        self.__counts += 1
        if self.__counts % self.__counts_per_file == 0: self.__getNewDataFile()
        print data
        self.__f.write(datetime.datetime.now().strftime("%Y%m%dT%H%M%S   "))
        self.__f.write(str(time.time())+'\n')
        sys.stdout.flush()

    def __getNewDataFile(self):
        self.__ofname=self.__datafnprefix+self.__uuds.pop()+self.__datafnpostfix
        self.__ofname=os.path.join(self.__datadir,self.__ofname)
        print self.__ofname
        return file(self.__ofname,'w')

    def start(self):
        self.__of=self.__getNewDataFile()
        self.__DAQ.run_forever()
        

if __name__ == '__main__':
    logfile=file('GMDAQ.log','a')
    mydaq=GMDAQ(logfile)
    if not mydaq.portOpen:
        logfile.close()
        sys.exit()
    try:
        mydaq.start()
    except KeyboardInterrupt:
        pass
    logfile.close()
    


