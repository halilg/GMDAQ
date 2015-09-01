#!/usr/bin/env python
import os, sys
from ROOT import TH1I, TCanvas

if __name__ == '__main__':

    c1=TCanvas()
    histo=TH1I("myhits","Hits;x;y",20,0,19)
    for interval in range(5, 30):
        f=file("data/data_53101eb8-14f1-41ea-9248-6299fb64ba1a.txt")
        histo.Reset()
        t0=0
        count=0
        for line in f.readlines():
            mytime=float(line.split()[1])
            if t0 == 0: t0 = mytime
            
            if mytime < t0+interval:
                count +=1
            else:
                t0=t0+interval
                histo.Fill(count)
                count=0
        
        histo.Draw()
        c1.Print("h_%02d.png" % interval)
        c1.Clear
        f.close()