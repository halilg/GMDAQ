#include "mylib.h"

//------------------------------------------------------------------------

#include <sys/stat.h>
// Function: fileExists
/**
    Check if a file exists
@param[in] filename - the name of the file to check

@return    true if the file exists, else false

*/
bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------

void dump_map(epoch_histo * ehisto){
    for( epoch_histo::iterator ii=ehisto->begin(); ii!=ehisto->end(); ++ii){
       std::cout << (*ii).first << ": " << (*ii).second << std::endl;
   }
   std::cout << "----------------\n";
}

//------------------------------------------------------------------------

const std::string currentDateTime(time_t now ) {
    //time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "t_{0} = %X - %Y/%m/%d", &tstruct);
    return buf;
}