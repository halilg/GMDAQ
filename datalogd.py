#!/usr/bin/env python
# Data logger daemon
# Receives data from named pipes and writes to data files

import os, sys, time, logging, logging.handlers
from daemon import runner

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
            if "Bluetooth" in port: continue 
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
        self.__DAQ.run_forever()





pidfname=os.path.join('/Users/halil/work/GMDAQ','mkdaemon.pid')
logfile=os.path.join('/Users/halil/work/GMDAQ','GMDAQ.log')
print pidfname
class App():
    
    def __init__(self):
        self.stdin_path = '/dev/null'
        self.stdout_path = logfile #'/Users/halil/work/GMDAQ/stdout'#'/dev/tty'
        self.stderr_path = logfile #'/Users/halil/work/GMDAQ/stderr' #'/dev/tty'
        self.pidfile_path =  pidfname
        self.pidfile_timeout = 5
            
    def run(self):
        while True:
            #Main code goes here ...
            #Note that logger level needs to be set to logging.DEBUG before this shows up in the logs
            logger.debug("Debug message")
            logger.info("Info message")
            logger.warn("Warning message")
            logger.error("Error message")
            time.sleep(10)

app = App()
logger = logging.getLogger("DaemonLog")
logger.setLevel(logging.INFO)
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
handler = logging.FileHandler(logfile)
handler.setFormatter(formatter)
logger.addHandler(handler)

daemon_runner = runner.DaemonRunner(app)
#This ensures that the logger file handle does not get closed during daemonization
daemon_runner.daemon_context.files_preserve=[handler.stream]
try:
    daemon_runner.do_action()
except runner.DaemonRunnerStopFailureError:
    print sys.argv[0],"is already stopped"
    
    

if __name__ == '__main__':
    persistentdata=trivialDB("datalogd.dat")
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
    formatter = logging.Formatter('%(asctime)s [%(name)s] - %(levelname)s : %(message)s')
    rfh.setFormatter(formatter)
    logger.addHandler(rfh)
    logger.setLevel(logging.DEBUG)
    logger.info("datalogd starting. Run #%05d" % runNumber)
    for npipe in os.listdir("."):
        if npipe [-3:] != ".io": continue
        print npipe
        #dlogger=GMDAQ(named_pipe, runNumber, logger)
        
    sys.exit()
    if not mydaq.portOpen:
        logger.error("To list available serial ports: python -m serial.tools.list_ports")
        sys.exit()
    try:
        mydaq.start()
    except KeyboardInterrupt:
        pass
    rfh.close()
