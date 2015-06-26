#!/usr/bin/env python

# prints the current run number
import os, sys
from trivialDB import trivialDB


if __name__ == '__main__':
        persistentdata=trivialDB("gmdaq.dat")
        runNumber=0
        try: runNumber = persistentdata.get("runNumber")
        except KeyError:
            sys.exit(1)

        print runNumber
        