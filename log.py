#!/usr/bin/env python
import sys,datetime

# A small and simple logging class
class slog:
    __lfname=None
    __lfile=None
    timestampformat="[%Y-%m-%d %H:%M:%S]"
    def __init__(self, lfname="testlog.txt"):
        self.__lfname=lfname
        self.__lfile=file(self.__lfname,'a')
        
    def __del__(self):
        self.__lfile.close()
    
    def getLogFileName(self):
        return self.__lfname
    
    def log(self,msg,category=""):
        timestamp=datetime.datetime.now().strftime(self.timestampformat)
        logtext = "%s %s%s\n" % (timestamp, category, msg)
        self.__lfile.write(logtext)

    def logInfo(self,msg):
        self.log(msg,"[I] ")
        
    def logWarn(self,msg):
        self.log(msg,"[W] ")
        
    def logErr(self,msg):
        self.log(msg,"[E] ")

if __name__ == '__main__':
    # soem code to test the log class
    mylog = log()
    mylog.log("Some message")
    mylog.logInfo("Info text")
    mylog.logWarn("Warning message")
    mylog.logErr("Error message")
    print "Wrote to:", mylog.getLogFileName()
