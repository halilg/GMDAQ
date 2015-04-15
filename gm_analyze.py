#!/usr/bin/env python
import os,sys,datetime,time

if len(sys.argv) == 1:
    print 'usage:', sys.argv[0],'<data file> [<data file>]..'
    sys.exit()

#fname="data_horiz0.txt"
#fname="data_vert0.txt"

data={}
for i in range(1,len(sys.argv)):
    nhits=0
    fname=sys.argv[i]   
    try:
        data_raw=file(fname).readlines()
    except:
        print 'Failed to open file:', fname
        sys.exit()
    for datum_raw in data_raw:
        nhits+=1
        epoch=float(datum_raw.split()[-1])
        a=datetime.datetime.fromtimestamp(epoch)
        #print a.minute
        if not data.has_key(a.year): data[a.year]={}
        if not data[a.year].has_key(a.month): data[a.year][a.month]={}
        if not data[a.year][a.month].has_key(a.day): data[a.year][a.month][a.day]={}
        if not data[a.year][a.month][a.day].has_key(a.hour): data[a.year][a.month][a.day][a.hour]={}
        if not data[a.year][a.month][a.day][a.hour].has_key(a.minute): data[a.year][a.month][a.day][a.hour][a.minute]=0
        
        #print a.year,a.month,a.day,a.minute
        data[a.year][a.month][a.day][a.hour][a.minute]=data[a.year][a.month][a.day][a.hour][a.minute]+1
            
        #print time.strftime('%Y-%m-%d %H:%M', time.localtime(moment))
print "Read",nhits,"hits in",i,"data files"

thismonthsdata=data[2015][4]
cnt=0.
csum=0.
for year in sorted(data.keys()):
    print year
    for month in sorted(data[year].keys()):
        print month
        for day in sorted(data[year][month].keys()):
            print day
            for hour in sorted(data[year][month][day].keys()):
                for minute in sorted(data[year][month][day][hour].keys()):
                    print '%02d:%02d %3d' % (int(hour), int(minute), data[year][month][day][hour][minute])
                    cnt+=1
                    csum+=data[year][month][day][hour][minute]

print "mean=",csum/cnt

        