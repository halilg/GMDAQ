#!/usr/bin/env python

import os,sys,stat, time

pipe_name = 'ipc.io'

class mkfifo:
    def __init__(self):
        pass

    def makeNPipe(self, pname):
        create = not (os.path.exists(pipe_name) and stat.S_ISFIFO(os.stat(pipe_name).st_mode))
        if create:
            try:
                os.mkfifo(pname)
            except OSError, e:
                #print "Failed to create FIFO: %s" % e
                return e
        
class transmitter(mkfifo):
    __out = None
    def __init__(self):
        self.makeNPipe(pipe_name)
        print "Waiting for connection..",
        sys.stdout.flush()
        self.__out = os.open(pipe_name, os.O_WRONLY)
        print "Connected"
    
    def send(self):
        try:
            while True:
                msg=raw_input('Message: ')
                os.write(self.__out, msg+'\n')
        except KeyboardInterrupt:
            os.close(self.__out)
            sys.exit()
            

class receiver(mkfifo):
    __pipein = None
    def __init__(self):
        self.makeNPipe(pipe_name)
        print "Waiting for connection..",
        sys.stdout.flush()
        self.__pipein = open(pipe_name, 'r')
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
    while True:
        a=raw_input('Transmitter (t) or receiver (r): ')
        if a == 't':
            t=transmitter()
            t.send()
        elif a == 'r':
            t=receiver()
            t.listen()
            