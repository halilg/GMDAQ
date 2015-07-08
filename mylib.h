#ifndef MYLIB_H
#define MYLIB_H

#include <sys/stat.h>
#include <string>
bool fileExists(const std::string& );

#include "epoch_histo.h"
#include <iostream>
void dump_map(epoch_histo * );

#include <time.h>
const std::string currentDateTime(time_t );

#include <sqlite3.h>
#include <string>
#include <cstdio>
void dumpTable(sqlite3*, std::string);

#endif