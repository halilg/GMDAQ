#!/usr/bin/env python
import serial,os,sys,datetime,time,uuid

import cPickle
class trivialDB():
    def __init__(self, dbname):
        self.__db=None
        self.__dbname=dbname
        self.__data={}
    
    def get(self, key):
        self.read()
        return self.__data[key]

    def put(self, key, value):
        self.__data[key] = value

    def read(self):
        try:
            self.__db = open(self.__dbname, 'rb')
            self.__data = cPickle.load(self.__db)
            self.__db.close()
        except IOError:
            self.write()

    def write(self):
        self.__db = open(self.__dbname, 'wb')
        cPickle.dump(self.__data, self.__db)
        self.__db.close()
    

if __name__ == '__main__':
    persistentdata=trivialDB("gmdaq.dat")
    runNumber=0
    try: runNumber = persistentdata.get("runNumber")
    except KeyError:
        pass
    
    print "last run number is:", runNumber
    rn=int(raw_input("Enter new last run rumber:"))
    persistentdata.put("runNumber",rn)
    persistentdata.write()
