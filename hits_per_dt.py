#!/usr/bin/env python
import os,sys,datetime,time

interval=10 #seconds

if len(sys.argv) == 1:
    print 'usage:', sys.argv[0],'<data file> [<data file>]..'
    sys.exit()

#fname="data_horiz0.txt"
#fname="data_vert0.txt"

hitdata=[]
nhits=0
for i in range(1,len(sys.argv)):
    fname=sys.argv[i]   
    try:
        data_raw=file(fname).readlines()
    except:
        print 'Failed to open file:', fname
        sys.exit()
    for datum_raw in data_raw:
        nhits+=1
        epoch=float(datum_raw.split()[-1])
        hitdata.append(epoch)#a=datetime.datetime.fromtimestamp(epoch)
        
        
print "Read",nhits,"hits in",i,"data files"

histogram={}
for q in range(100):
    histogram[q]=0

hitdata=sorted(hitdata)
epstart=hitdata[0]
cnt=0
for i in range(1,len(hitdata)):
    if hitdata[i] < epstart + interval:
        cnt+=1
    else:
        #print cnt
        histogram[cnt]+=1
        cnt=1
        epstart=epstart + interval

#print sorted(hitdata)

print histogram