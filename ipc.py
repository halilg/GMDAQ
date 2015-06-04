#!/usr/bin/env python

import os,sys,stat, time

class mkfifo:
    def __init__(self):
        pass

    def makeNPipe(self, pname):
        create = not (os.path.exists(pname) and stat.S_ISFIFO(os.stat(pname).st_mode))
        if create:
            try:
                os.mkfifo(pname)
            except OSError, e:
                #print "Failed to create FIFO: %s" % e
                return e
        
class transmitter(mkfifo):
    __pipeout = None
    __pipe_name = None
    __logger=None
    
    def __init__(self, logger, pn="fifo.io"):
        self.__logger = logger
        self.__pipe_name = pn
        self.makeNPipe(self.__pipe_name)
        self.__logger.debug('Waiting for named pipe connection: "%s"' % self.__pipe_name)
        self.__pipeout = os.open(self.__pipe_name, os.O_WRONLY)
        self.__logger.info('Connection established to name pipe: "%s"' % self.__pipe_name )
    
    def __del__(self):
        self.close()
        
    def close(self):
        os.close(self.__pipeout)
        os.remove(self.__pipe_name)
        self.__logger.debug("Pipe closed: %s" % self.__pipe_name)
    
    def send(self, msg=""):
        try:
            if not msg:
                while True:
                    msg = raw_input('Message: ')
                    os.write(self.__pipeout, msg+'\n')
            else:
                os.write(self.__pipeout, msg+'\n')
        except KeyboardInterrupt:
            sys.exit()
            

class receiver(mkfifo):
    __pipein = None
    __pipe_name = None
    __logger=None
    
    def __init__(self, logger, pn="fifo.io"):
        self.__logger = logger
        self.__pipe_name = pn
        self.makeNPipe(self.__pipe_name)
        self.__logger.debug("Waiting for connection..")
        self.__pipein = open(self.__pipe_name, 'r')
        self.__logger.info("Connected")
    
    def listen(self):
        while True:
            try:
                line = self.__pipein.readline()
                if line == '': # other party left
                    self.__pipein.close()
                    sys.exit()
            except KeyboardInterrupt:
                self.__pipein.close()
                sys.exit()
            print 'Received %s' % (line.strip())

    
if __name__ == '__main__':
    pipe_name = 'ipc.io'
    while True:
        a=raw_input('Transmitter (t) or receiver (r): ')
        if a == 't':
            t=transmitter(pipe_name)
            t.send()
        elif a == 'r':
            t=receiver(pipe_name)
            t.listen()
            