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
    
    def __init__(self, pn):
        self.__pipe_name = pn
        self.makeNPipe(self.__pipe_name)
        print "Waiting for connection..",
        sys.stdout.flush()
        self.__pipeout = os.open(self.__pipe_name, os.O_WRONLY)
        print "Connected"
    
    def send(self):
        try:
            while True:
                msg=raw_input('Message: ')
                os.write(self.__pipeout, msg+'\n')
        except KeyboardInterrupt:
            os.close(self.__pipeout)
            sys.exit()
            

class receiver(mkfifo):
    __pipein = None
    __pipe_name = None

    def __init__(self, pn):
        self.__pipe_name = pn
        self.makeNPipe(self.__pipe_name)
        print "Waiting for connection..",
        sys.stdout.flush()
        self.__pipein = open(self.__pipe_name, 'r')
        print "Connected"
    
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
            