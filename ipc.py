#!/usr/bin/env python

import os,sys, time

pipe_name = 'ipc.io'

class transmitter:
    __out = None
    def __init__(self):
        try:
            os.mkfifo(pipe_name)
        except OSError, e:
            print "Failed to create FIFO: %s" % e
        else:
            self.__out = open(pipe_name, 'a')
    
    def send(self):
        try:
            while True:
                msg=raw_input('Message: ')
                print >> self.__out, msg
        except KeyboardInterrupt:
            try: self.__out.close()
            except: pass

class receiver:
    def __init__(self):
        self.__pipein = open(pipe_name, 'r')
    
    def listen(self):
        while True:
            line = self.__pipein.readline()[:-1]
            print 'Received %s' % (line)

    
if __name__ == '__main__':
    while True:
        a=raw_input('Transmitter (t) or receiver (r): ')
        if a == 't':
            t=transmitter()
            t.send()
        elif a == 'r':
            t=receiver()
            t.listen()
            