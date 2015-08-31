#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,sys,re, sqlite3, time


dataDir="data"
p = re.compile("gm_\d\d\d\d\d\.dat", re.IGNORECASE)
files=os.listdir(dataDir)
files= filter(p.match, files)

runsdata={}

def epoch2DateTime(ep):
    return time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(ep))

def readRunsData():
    con = None
    for dbfile in files:
        #print dbfile
        runsdata[dbfile] = [-1, "?", "?"]
        try:
            con = sqlite3.connect(os.path.join(dataDir,dbfile))
            cur = con.cursor()
            # cur.execute("SELECT name FROM sqlite_master WHERE type='table';")
            # print(cur.fetchall())
            cur.execute("SELECT * FROM log")
            alldata=cur.fetchall()
            runsdata[dbfile] = [len(alldata), epoch2DateTime(alldata[0][0]/1000.), epoch2DateTime(alldata[-1][0]/1000.)]
            # print runsdata
             
            
            
        except sqlite3.Error, e:
            pass
            #print "Error %s:" % e.args[0]
            # sys.exit(1)
            
        finally:
            
            if con:
                con.close()


readRunsData()

print "Run#          #Hits                 Start                   End"
print "---------------------------------------------------------------"  
for k in sorted(runsdata.keys()):
    print "%2s       %7d   %19s   %19s" % (k[3:8], runsdata[k][0], runsdata[k][1], runsdata[k][2])
