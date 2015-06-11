#!/usr/bin/env python

# Data acquisition daemon

import serial,os,sys,datetime,time,uuid,pwd, platform#, psutil
import logging, logging.handlers, lockfile, signal, threading
from serial_ports import serial_ports
from daemon import runner
from trivialDB import trivialDB

SBAUD=115200

sig_names = {1: 'SIGHUP', 2: 'SIGINT', 3: 'SIGQUIT', 4: 'SIGILL', 5: 'SIGTRAP', 6: 'SIGIOT', 7: 'SIGEMT', 8: 'SIGFPE', 9: 'SIGKILL', 10: 'SIGBUS', 11: 'SIGSEGV', 12: 'SIGSYS', 13: 'SIGPIPE', 14: 'SIGALRM', 15: 'SIGTERM', 16: 'SIGURG', 17: 'SIGSTOP', 18: 'SIGTSTP', 19: 'SIGCONT', 20: 'SIGCHLD', 21: 'SIGTTIN', 22: 'SIGTTOU', 23: 'SIGIO', 24: 'SIGXCPU', 25: 'SIGXFSZ', 26: 'SIGVTALRM', 27: 'SIGPROF', 28: 'SIGWINCH', 29: 'SIGINFO', 30: 'SIGUSR1', 31: 'SIGUSR2'}

#http://www.tutorialspoint.com/python/python_multithreading.htm
class SerialComm(threading.Thread):   
    def __init__(self, threadID, name, ser, dlogfile):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.__logger = logging.getLogger("%s.%s" % ( self.__module__, self.__class__.__name__ ))
        self.__serial=ser
        self.__ofname=dlogfile
        self.__ofile=open(dlogfile,"w",0) # buffer size=0

    def run(self):
        self.__logger.info("Thread %1d starting: %s" % (self.threadID, self.name))
        self.__logger.info("Logging data to: %s" % self.__ofname)
        try:
            while True:
                data = self.__serial.readline().strip()
                msecs=time.time()*1000
                data = "%12d   %s\n" % (msecs, data) # time stamp
                self.__ofile.write(data)
        except serial.SerialException:
            pass
        self.stop()
        self.__logger.info("Thread exiting: %s" % self.name)
    
    def stop(self):
        if self.__ofile: self.__ofile.close()
        #if self.__serial.isOpen(): self.__serial.close()

class App():
    # The class that is the daemon. Steers the things
    def __init__(self, pidfname):
        self.MyName="datackd"
        self.__logger = logging.getLogger("%s.%s" % ( self.__module__, self.__class__.__name__ ))
        self.workdir=os.path.dirname(logger.handlers[0].baseFilename)
        self.stdin_path = '/dev/null'
        self.stdout_path = '/dev/tty' #logfile
        self.stderr_path = '/dev/tty' #logfile
        self.stdout_path = logfile
        self.stderr_path = logfile
        self.pidfile_path =  pidfname
        self.pidfile_timeout = 5
        self.serial_connections=[]
        self.workers=[]        
        self.__datadir="data"
        self.__datafnprefix=''
        self.__datafnpostfix='.txt'

    def signal_handler(self, signum, frame):
        self.__logger.debug("Received signal %s" % sig_names[signum])
        if signum == 15: # SIGTERM
            self.__stopping = True
            self.stop()
            self.__logger.debug("Exiting daemon")
            sys.exit()

    def run(self):
        signal.signal(signal.SIGTERM, self.signal_handler)
        
        # Script needs maximum process priority to timestamp incoming data accurately
        if os.getuid() != 0:
            self.__logger.warning("Not started as root user")
                                  
        try:
            print os.nice(-19)
            self.__logger.debug("Self-renice successful")
        except OSError:
            if not "Darwin" in platform.platform(): # os.nice(-n) didn't work on Mac OS no matter what @@@
                self.__logger.warning("Renice failed. Starting this daemon with superuser priviledges might help it work.")

        # Create the data directory if it doesn't exist
        datadirabs=os.path.join(self.workdir,self.__datadir)
        if not (os.path.exists(datadirabs) and os.path.isdir(datadirabs) ):
            try: os.mkdir (datadirabs)
            except:
                self.__logger.error( ("Failed to create data directory: %s") % datadirabs)
                return
        
        #determine the run number
        persistentdata=trivialDB(os.path.join(self.workdir,self.MyName+".dat"))
        runNumber=0
        try: runNumber = persistentdata.get("runNumber")
        except KeyError:
            pass        
        runNumber += 1
        self.__logger.info("Starting run %05d" % runNumber)
        persistentdata.put("runNumber",runNumber)
        persistentdata.write()
        
        serialports=serial_ports()
        self.__logger.debug("Available serial ports: %s" % serialports)

        ser=None
        threadID=0
        for port in serialports:
            if "Bluetooth" in port: continue
            try:
                self.__logger.debug("Opening serial port: %s" % port)
                ser = serial.Serial(port, SBAUD)
                if ser.isOpen():
                    self.serial_connections.append(ser)
                    self.__logger.debug('Serial port opened: %s' %(port))
                    data = ser.readline().strip()
                    self.__logger.debug('Received: %s' %(data))
                    tname="?"
                    dprefix="data"
                    if data == "!IAM AUNO":
                        tname="GMLogger"
                        dprefix="gm"
                    elif data == "!IAM ADUE":
                        tname="EnvLogger"
                        dprefix="env"
                        

                    ofname=self.__datafnprefix+dprefix+str("_%05d" % runNumber)+self.__datafnpostfix
                    ofname=os.path.join(self.workdir, self.__datadir, ofname)
                    t = SerialComm(threadID, tname,ser, ofname)
                    threadID+=1
                    t.daemon = False
                    self.workers.append(t)
                        
                else:
                    self.__logger.error('Serial port open timeout')
                    
            except OSError:
                self.__logger.error("Serial port couldn't be opened: %s" % (port))
                continue

        if len(self.workers) == 0:
            self.__logger.warning("No compatible serial device found. Exiting.")
            return

            
        for t in self.workers:
            t.start()

        while True:
            time.sleep(1000)# Exiting the function means terminating the daemon 
            
    def stop(self):
        self.__logger.debug("Stopping")
        
        for ser in self.serial_connections:
            #http://pyserial.sourceforge.net/pyserial_api.html
            if ser.isOpen():
                ser.close()
                self.__logger.debug("Connection closed: %s ", ser.name)
            
        
            
        for worker in self.workers:
            #http://pymotw.com/2/multiprocessing/basics.html
            if worker.is_alive():
                self.__logger.debug("terminating: %s" % worker.name)
                worker.terminate()
                worker.join()  
        


if __name__ == '__main__':
    myname=os.path.basename(sys.argv[0])
    
    #set-up paths and filenames
    scriptpath=os.path.realpath(__file__)
    wpath=os.path.dirname(scriptpath) # set working directory to the path of the script
    #wpath=os.getcwd() # uncomment to set the working directory to $PWD
    pidfname=os.path.join(wpath,myname+".pid")
    logfile=os.path.join(wpath,myname+".log")
    
    if len(sys.argv) > 1:
        if sys.argv[1] == "status":
            if os.path.exists(pidfname) :
                pid=open(pidfname).readline()
                print pid,
            else:
                print myname,"is not running"
            sys.exit()
    
    #set-up logger
    logger = logging.getLogger(__name__)
    rfh = logging.handlers.RotatingFileHandler(logfile,
                                               maxBytes=100000,
                                               backupCount=100,
                                               )
    logger.addHandler(rfh)
    #logger.setLevel(logging.INFO)
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    rfh.setFormatter(formatter)
    
    # http://www.gavinj.net/2012/06/building-python-daemon-process.html
    duid=os.stat(scriptpath).st_uid
    username=pwd.getpwuid(duid).pw_name
    app = App(pidfname)
    logger.debug("Will run as user: %s (%d)" % (username, duid))
    daemon_runner = runner.DaemonRunner(app)
    daemon_runner.daemon_context.uid=duid
    
    #This ensures that the logger file handle does not get closed during daemonization
    daemon_runner.daemon_context.files_preserve=[rfh.stream]
    
    try:
        daemon_runner.do_action()
    except runner.DaemonRunnerStopFailureError:
        print myname,"is already stopped"
    except lockfile.LockTimeout:
            print "Another instance of %s is running" % myname
            sys.exit()
