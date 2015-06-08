#!/usr/bin/env python
# Data logger daemon
# Receives data from named pipes and writes to data files

import os, sys, time, logging, logging.handlers, stat
import threading
from daemon import runner
#from ipc import *

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

class LogData(threading.Thread):
    __ofname=''
    __path="."
    __datadir="data"
    __datafnprefix=''
    __datafnpostfix='.txt'
    __counts_per_file=12000
    __counts=0
    __f=None
    __logfile = None
    __ofnum=0
    portOpen = False
    
    def __init__(self, npipe, runNumber, logger):
        threading.Thread.__init__(self)
        self.__logger = logger
        self.__runNumber = runNumber
        #self.__npipe = npipe
        self.__DAQ=open(npipe)
        self.__path = os.path.dirname(npipe)
        self.__datadir = os.path.join(self.__path,self.__datadir)
        self.__datafnprefix=os.path.basename(npipe).replace(".io", "")
                
        # Create the data directory if it doesn't exist
        if not (os.path.exists(self.__datadir) and os.path.isdir(self.__datadir) ):
            try: os.mkdir (self.__datadir)
            except:
                self.__logger.error( ("Failed to create directory: %s") % self.__datadir)
                return
        
    def __record_data(self, data):
        self.__counts += 1
        if self.__counts % self.__counts_per_file == 0:
            self.__of = self.__getNewDataFile()
            self.__logger.info( ("%d hits, writing to new data file: %s") % (self.__counts, self.__ofname) )
        self.__of.write(datetime.datetime.now().strftime("%Y%m%dT%H%M%S   "))
        secs=time.time()*1000
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
        while True:
            data=self.__DAQ.readline()
            self.__record_data(data)

class MultiLoggerDaemon():
    def __init__(self, logger, pidfname, runnumber):
        self.stdin_path = '/dev/null'
        temp = logger.handlers[0].baseFilename
        self.stdout_path = temp #'/Users/halil/work/GMDAQ/stdout'#'/dev/tty'
        self.stderr_path = temp #'/Users/halil/work/GMDAQ/stderr' #'/dev/tty'
        self.pidfile_path =  pidfname
        self.pidfile_timeout = 5
        self.__logger = logger
        self.__runNumber= runnumber
        
            
    def run(self):
        wdir=os.path.dirname(self.pidfile_path)
        dloggers=[]
        #                if stat.S_ISFIFO(os.stat(os.path.join(wdir,fname)).st_mode):
        
        npipe=os.path.join(wdir,"env.io")
        self.__logger.info("Connecting to FIFO: %s", npipe)
        dloggers.append(LogData(npipe, self.__runNumber, self.__logger))
        dloggers[-1].setDaemon(True)
        print "1"
        dloggers[-1].start()
        print "2"
        time.sleep(5)
        print "3"
        npipe=os.path.join(wdir,"gm.io")
        self.__logger.info("Connecting to FIFO: %s", npipe)
        dloggers.append(LogData(npipe, self.__runNumber, self.__logger))
        dloggers[-1].setDaemon(True)
        dloggers[-1].start()

    

if __name__ == '__main__':
    persistentdata=trivialDB("datalogd.dat")
    pidfname=os.path.join('/Users/halil/work/GMDAQ','datalogd.pid')
    runNumber=0
    try: runNumber = persistentdata.get("runNumber")
    except KeyError:
        pass
    
    runNumber += 1
    persistentdata.put("runNumber",runNumber)
    persistentdata.write()

    logger = logging.getLogger('datalogd')
    rfh = logging.handlers.RotatingFileHandler('GMDAQ.log',
                                               maxBytes=100000,
                                               backupCount=100,
                                               )
    #fh = logging.FileHandler()
    formatter = logging.Formatter('%(asctime)s - %(levelname)s : [%(name)s] %(message)s')
    rfh.setFormatter(formatter)
    logger.addHandler(rfh)
    logger.setLevel(logging.DEBUG)
    logger.info("Starting. Run #%05d" % runNumber)
    #for npipe in os.listdir("."):
    #    if npipe [-3:] != ".io": continue
    #    print npipe     
    
    app = MultiLoggerDaemon(logger, pidfname, runNumber)
        
    daemon_runner = runner.DaemonRunner(app)
    #This ensures that the logger file handle does not get closed during daemonization
    daemon_runner.daemon_context.files_preserve=[rfh.stream]
    try:
        daemon_runner.do_action()
    except runner.DaemonRunnerStopFailureError:
        print sys.argv[0],"is already stopped"




