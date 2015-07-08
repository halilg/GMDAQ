#include "mylib.h"

//------------------------------------------------------------------------

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
    std::cout << "----- Dumping map -----------\n";
    for( epoch_histo::iterator ii=ehisto->begin(); ii!=ehisto->end(); ++ii){
       std::cout << (*ii).first << ": " << (*ii).second << std::endl;
    }
    std::cout << "------- #elements: " <<  ehisto->size() << " ---------\n";
}

//------------------------------------------------------------------------

void dumpTable(sqlite3* mydb, std::string table){
    sqlite3_stmt *statement;    
    char buff[100];
    sprintf(buff, "select * from %s", table.c_str());
    unsigned int nrecords=0;
    if ( sqlite3_prepare(mydb, buff, -1, &statement, 0 ) == SQLITE_OK ) 
    {        
        int ctotal = sqlite3_column_count(statement);
        int res = 0;
        std::cout << "Dumping sqlite table: \"" << table << "\" (" << ctotal << " columns)\n-----\n";
        while ( 1 )         
        {
            res = sqlite3_step(statement);

            if ( res == SQLITE_ROW ) 
            {
                ++nrecords;
                for ( int i = 0; i < ctotal; i++ ) 
                {
                    std::string s = (char*)sqlite3_column_text(statement, i);
                    // print or format the output as you want 
                    std::cout << s << " " ;
                }
                std::cout << std::endl;
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR)    
            {
                std::cout << "-----\n " << nrecords << " records.\n----- \\ table dump" << std::endl;
                break;
            }    
        }
    } else {
        std::cout << "Dumping sqlite table: \"" << table << "\" failed.\n";
        std::cout << sqlite3_errmsg(mydb) << " (" << sqlite3_errstr(sqlite3_extended_errcode(mydb)) << ")" << std::endl;
    }
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

