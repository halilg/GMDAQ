#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,sys,re, sqlite3, time, getopt
from trivialDB import trivialDB

# Run/data management tools

p = re.compile("gm_\d\d\d\d\d\.dat$", re.IGNORECASE)

def epoch2DateTime(ep):
    return time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(ep))

def getRowCount(con, table):
    cur = con.cursor()
    cur.execute("select count() from %s" % table)
    alldata=cur.fetchall()
    return alldata[0][0]

def getFLepochMS(con, table):
    # returns the first and last epochms in a db table
    cur = con.cursor()
    cur.execute("SELECT * FROM %s ORDER BY epochms ASC limit 1" % table)
    firstepochms=cur.fetchall()[0][0]
    cur.execute("SELECT * FROM %s ORDER BY epochms DESC limit 1" % table)
    lastepochms=cur.fetchall()[0][0]
    return firstepochms, lastepochms

def readRunData(fname):
    rundata = [-1, "?", "?", 0]
    try:
        con = sqlite3.connect(fname)
        rowscnt=getRowCount(con, "log")
        firstepochms, lastepochms=getFLepochMS(con, "log")
        # print firstepochms, lastepochms
        # continue
        if rowscnt > 0:
            rundata = [rowscnt, epoch2DateTime(firstepochms/1000.), epoch2DateTime(lastepochms/1000.), \
                                (lastepochms-firstepochms)/1000./60./60./24.]
        else:
            rundata = [0, 'N/A', 'N/A', 0]
        
    except sqlite3.Error, e:
        pass
        
    finally:
        
        if con:
            con.close()
    return rundata

def readRunsData(dataDir):
    runsdata={}
    con=None
    files=os.listdir(dataDir)
    files=filter(p.match, files)
    rowscnt=0
    for dbfile in files:
        rundata=readRunData(os.path.join(dataDir,dbfile))
        runsdata[dbfile]=rundata
    return runsdata

def whichRun():
    persistentdata=trivialDB("gmdaq.dat")
    runNumber=0
    try: runNumber = persistentdata.get("runNumber")
    except KeyError:
        sys.exit(1)

    return runNumber

def help(myname):
    print "Usage:", myname, '<-l> <-c> <-r run number> <-d run number>'

if __name__ == "__main__":
    dataDir="data"
    try:
        opts, args = getopt.getopt(sys.argv[1:],"clr:d:")
        # print opts, args
    except getopt.GetoptError:
        help(sys.argv[0])
        sys.exit(2)
    
    # print opts
    if len(opts) == 0:
        help(sys.argv[0])
        sys.exit()
        
    for opt, arg in opts:
        if opt == '-c': # Print current run number
            print whichRun()
            sys.exit()
            
        elif opt == '-l': # List available runs
            runsdata=readRunsData(dataDir)
            print "Run#                 #Hits                 Start                   End     Days"
            print "-------------------------------------------------------------------------------"  
            for k in sorted(runsdata.keys()):
                try: print "%2s       %7d   %19s   %19s     %4.1f" % (k, runsdata[k][0], runsdata[k][1], runsdata[k][2], runsdata[k][3])
                except: print runsdata[k]
                
        elif opt == "-r": # Print metadata for a single run
            runsdata=readRunsData(dataDir)
            runnum=int(arg)
            k="gm_%05d.dat" % runnum
            try: print "%2s       %7d   %19s   %19s     %4.1f" % (k, runsdata[k][0], runsdata[k][1], runsdata[k][2], runsdata[k][3])
            except KeyError:
                print "No such run:" , runnum
        
        elif opt == "-d": # Delete run
            runnum=int(arg)
            k="*%05d*" % runnum
            print "Delete all data of run #%05d ? (N/y):" % runnum, 
            inp=raw_input()
            if inp == "y":
                # os.system("ls %s" % os.path.join(dataDir,k))
                os.system("rm %s" % os.path.join(dataDir,k)) 
