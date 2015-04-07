#!/usr/bin/env python

import os,sys,stat, time

pipe_name = 'ipc.io'

class transmitter:
    __out = None
    def __init__(self):
        create = not (os.path.exists(pipe_name) and stat.S_ISFIFO(os.stat(pipe_name).st_mode))
        if create:
            try:
                os.mkfifo(pipe_name)
            except OSError, e:
                print "Failed to create FIFO: %s" % e
                sys.exit()
        self.__out = os.open(pipe_name, os.O_WRONLY)
    
    def send(self):
        try:
            while True:
                msg=raw_input('Message: ')
                os.write(self.__out, msg+'\n')
        except KeyboardInterrupt:
            os.close(self.__out)
            sys.exit()
            

class receiver:
    def __init__(self):
        self.__pipein = open(pipe_name, 'r')
    
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
            