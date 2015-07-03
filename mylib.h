#ifndef MYLIB_H
#define MYLIB_H

#include <string>
bool fileExists(const std::string& );

#include "epoch_histo.h"
#include <iostream>
void dump_map(epoch_histo * );

#include <time.h>
const std::string currentDateTime(time_t );

#endif