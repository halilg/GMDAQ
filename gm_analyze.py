#!/usr/bin/env python
import os,sys,datetime,time


fname="data_bent0.txt"
#fname="data_horiz0.txt"
#fname="data_vert0.txt"

data_raw=file(fname).readlines()

data={}

for datum_raw in data_raw:
    moment=float(datum_raw.split()[-1])
    a=datetime.datetime.fromtimestamp(moment)
    #print a.minute
    if not data.has_key(a.year): data[a.year]={}
    if not data[a.year].has_key(a.month): data[a.year][a.month]={}
    if not data[a.year][a.month].has_key(a.day): data[a.year][a.month][a.day]={}
    if not data[a.year][a.month][a.day].has_key(a.hour): data[a.year][a.month][a.day][a.hour]={}
    if not data[a.year][a.month][a.day][a.hour].has_key(a.minute): data[a.year][a.month][a.day][a.hour][a.minute]=0
    
    #print a.year,a.month,a.day,a.minute
    data[a.year][a.month][a.day][a.hour][a.minute]=data[a.year][a.month][a.day][a.hour][a.minute]+1
        
    #print time.strftime('%Y-%m-%d %H:%M', time.localtime(moment))


thismonthsdata=data[2015][4]
cnt=0.
csum=0.
for day in sorted(thismonthsdata.keys()):
    for hour in sorted(thismonthsdata[day].keys()):
        for minute in sorted(thismonthsdata[day][hour].keys()):
            print str(hour)+":"+str(minute), thismonthsdata[day][hour][minute]
            cnt+=1
            csum+=thismonthsdata[day][hour][minute]

print "mean=",csum/cnt
print fname
        