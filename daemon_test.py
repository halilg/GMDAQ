#!/usr/bin/env python
import os, sys, signal
import daemon
import time
import lockfile
from daemon.pidfile import PIDLockFile



pidfname=os.path.join('/Users/halil/work/GMDAQ','mkdaemon.pid')

def do_something():
    while True:
        with open("/tmp/current_time.txt", "w") as f:
            f.write("The time is now " + time.ctime())
        time.sleep(5)

def run():
    context=daemon.DaemonContext(
        working_directory='/Users/halil/work/GMDAQ',
        umask=0o002,
        pidfile=lockfile.FileLock(pidfname),
    )
    
    with context:
        do_something()

def getPID():
    try:
        return int(file(pidfname).read().strip())
    except IOError:
        return -1

if __name__ == "__main__":
   # myname=sys.argv[0]
   # if len(sys.argv) > 1:
   #     if sys.argv[1] == "status":
   #         if os.path.exists(pidfname):
   #             print myname,"is running with PID", getPID()
   #         else:
   #             print myname,"is not running"
   #         sys.exit()
   #     elif sys.argv[1] == "stop":
   #         pid=getPID()
   #         if pid == -1: sys.exit()
   #         print "killing", pid
   #         os.kill(pid, signal.SIGQUIT) #or signal. SIGQUIT SIGKILL
   #         if getPID() != -1: print "Failed to kill", pid
   #         sys.exit()
   #     elif sys.argv[1] == "start":
   #         pass
    #print "starting"
    run()