#!/usr/bin/env python

import os, sys, time

start=time.time()
from ROOT import TFile, TNtuple, gRandom
end=time.time()
print "ROOT import time:", end-start

rf=TFile("test.root","RECREATE");
rn=rf.Get("/ntuple")
print rn.GetEntries() 