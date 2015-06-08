#!/usr/bin/env python
#standard python libs
import os, sys, time, logging

#third party libs
from daemon import runner

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
