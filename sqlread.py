#!/usr/bin/env python

import os, sys, time, random
import sqlite3

conn = sqlite3.connect('gm.db')

#start=time.time()

#end=time.time()
#print "ROOT import time:", end-start

c = conn.cursor()

c.execute('SELECT * FROM gm')
for (epoch, data) in c.execute('SELECT * FROM LOG'):
        print epoch, data

c.close()