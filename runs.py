#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,sys,re, sqlite3, time


dataDir="data"
p = re.compile("gm_\d\d\d\d\d\.dat$", re.IGNORECASE)
files=os.listdir(dataDir)
files= filter(p.match, files)

runsdata={}

def epoch2DateTime(ep):
    return time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(ep))

def readRunsData():
    con = None
    for dbfile in files:
        # print dbfile
        runsdata[dbfile] = [-1, "?", "?", 0]
        try:
            con = sqlite3.connect(os.path.join(dataDir,dbfile))
            cur = con.cursor()
            # cur.execute("SELECT name FROM sqlite_master WHERE type='table';")
            # print(cur.fetchall())
            cur.execute("SELECT * FROM log")
            alldata=cur.fetchall()
            if len(alldata) > 0:
                runsdata[dbfile] = [len(alldata), epoch2DateTime(alldata[0][0]/1000.), epoch2DateTime(alldata[-1][0]/1000.), \
                                    (alldata[-1][0]-alldata[0][0])/1000./60./60./24.]
            else:
                runsdata[dbfile] = [0, 'N/A', 'N/A', 0]
            # print runsdata
             
            
            
        except sqlite3.Error, e:
            pass
            #print "Error %s:" % e.args[0]
            # sys.exit(1)
            
        finally:
            
            if con:
                con.close()


readRunsData()

print "Run#                 #Hits                 Start                   End     Days"
print "-------------------------------------------------------------------------------"  
for k in sorted(runsdata.keys()):
    try: print "%2s       %7d   %19s   %19s     %4.1f" % (k, runsdata[k][0], runsdata[k][1], runsdata[k][2], runsdata[k][3])
    except: print runsdata[k]
