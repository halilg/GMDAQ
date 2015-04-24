#!/usr/bin/env python
import os, sys
from ROOT import TH1I, TCanvas, gStyle

if __name__ == '__main__':

    c1=TCanvas()
    histo=TH1I("myhits","GM Counter Hits in the Last 24 Hours;t(Hr);Hits",23,-23,0)
    gStyle.SetOptStat(0);
    interval=3600 #seconds
    f=file("test_data/data_6e2d8108-725a-4ed3-add2-05a31e1af660.txt")
    
    count=0
    data=[]
    for line in f.readlines():
        mytime=float(line.split()[1])
        data.append(mytime)
    data=sorted(data)
    data=data[::-1] #reverse list
    t0=data[0]
    
    
    hour=0
    for mytime in data:
        #print mytime
        if mytime > t0+(hour-1)*interval:
            histo.Fill(hour)
            #print hour
        else:
            hour -= 1
            if hour < -23: break
    
    histo.Draw("e")
    c1.Print("h_24h.png")
    c1.Clear
    f.close()