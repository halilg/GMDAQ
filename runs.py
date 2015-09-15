#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,sys,re, sqlite3, time, getopt
from trivialDB import trivialDB

# Run/data management tools

p = re.compile("gm_\d\d\d\d\d\.dat$", re.IGNORECASE)

def epoch2DateTime(ep):
    return time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(ep))

def readRunsData(dataDir):
    runsdata={}
    con=None
    files=os.listdir(dataDir)
    files=filter(p.match, files)
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
            
        except sqlite3.Error, e:
            pass
            
        finally:
            
            if con:
                con.close()
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
