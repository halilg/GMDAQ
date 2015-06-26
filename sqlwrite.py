#!/usr/bin/env python

import os, sys, time, random
import sqlite3

conn = sqlite3.connect('gm.db')

#start=time.time()

#end=time.time()
#print "ROOT import time:", end-start

c = conn.cursor()
c.execute("CREATE TABLE if not exists gm (epochms INTEGER, millis INTEGER)")
c.execute("PRAGMA synchronous=OFF") #improves the write time about 1 ms (which is about 2ms)

print "Write delay (ms):\n----------------------"
try:
    for q in range(1000):
        epoch=time.time()*1000
        millis=epoch
        
        start=time.time()*1000
        cmd="INSERT INTO gm VALUES (%d, %d)" % (epoch, millis)
        
        c.execute(cmd)
        conn.commit()
        print time.time()*1000 - start
        time.sleep(5*random.random()) #secs
except KeyboardInterrupt:
    pass

c.close()
