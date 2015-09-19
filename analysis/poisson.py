#!/usr/bin/env python
import os, sys
from ROOT import TH1I, TCanvas, TFile, TMath, TF1

if __name__ == '__main__':
    c1=TCanvas()
    rf=TFile('histos.root',"recreate")

    f1 = TF1("f1","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)", 0, 20); #// "xmin" = 0, "xmax" = 10
    f1.SetParameters(1, 1, 1)#; // you MUST set non-zero initial values for parameters
    
    
    for interval in range(5, 30):
        f=file("../data/data_53101eb8-14f1-41ea-9248-6299fb64ba1a.txt")
        of=file("data_%02d.txt" % interval,"w")
        histo=TH1I("hits_%02d" % interval,"Hits;x;y",20,0,19)
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
                of.write(str(count)+"\n")
                count=0
        
        histo.Draw()
        histo.Fit("f1", "R"); #// "R" = fit between "xmin" and "xmax" of the "f1"
        c1.Print("h_%02d.png" % interval)
        c1.Clear
        f.close()
        rf.Write()
        of.close()
    rf.Close()