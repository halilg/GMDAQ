#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys
import sqlite3 #runs


if __name__ == '__main__':
    
    if len(sys.argv) > 1:
        k=sys.argv[1]
    else:
        print 'usage:',os.path.basename(sys.argv[0]),'<file name>'
        sys.exit()
    
    con = sqlite3.connect(k)
    cursor = con.cursor()
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
    
    tables=cursor.fetchall()
    if len(tables) == 0:
        print 'No tables found'
        con.close()
        sys.exit()    
    
    else:
        tables=tables[0]
        print 'Table(s): ', ', '.join(tables)

    for table in tables:
        print 'Dumping table:', table
        cursor.execute("SELECT * FROM %s" % table)
        rows=cursor.fetchall()
        for row in rows:
            print row
        
        print 'Table had %s rows ' % len (rows)
    con.close()