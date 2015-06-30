#!/usr/bin/env python

import os, sys, time, random
import sqlite3

fname = '/home/halil/work/GMDAQ/data/env_00000.dat'

if len (sys.argv) > 1: fname = sys.argv[1]


conn = sqlite3.connect(fname)

#start=time.time()

#end=time.time()
#print "ROOT import time:", end-start

c = conn.cursor()

c.execute('SELECT * FROM LOG')
for (epoch, data) in c.execute('SELECT * FROM LOG'):
        print epoch, data

c.close()