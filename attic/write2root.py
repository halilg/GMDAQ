#!/usr/bin/env python

import os, sys, time

start=time.time()
from ROOT import TFile, TNtuple, gRandom
end=time.time()
print "ROOT import time:", end-start


rf=TFile("test.root","RECREATE");
rf.cd()
rn =TNtuple("ntuple","ntuple","epoch:msec", 160)
rn.Write()
for q in range(1000):
    rn.Fill(gRandom.Gaus(1,1),gRandom.Gaus(1,1))
    time.sleep(2) #secs
rf.cd()
rn.Write()
rf.Close()