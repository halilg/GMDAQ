#!/usr/bin/env python

import os, sys, time, random
import sqlite3

conn = sqlite3.connect('/home/halil/work/GMDAQ/data/env_00000.dat')

#start=time.time()

#end=time.time()
#print "ROOT import time:", end-start

c = conn.cursor()

c.execute('SELECT * FROM LOG')
for (epoch, data) in c.execute('SELECT * FROM LOG'):
        print epoch, data

c.close()