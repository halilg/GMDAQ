#ifndef SQLRW_H
#define SQLRW_H

#include <string>
#include <sqlite3.h>
#include "epoch_histo.h"

class sqlrw {
    private:
        sqlite3 *db;
        bool fileopened;
        char *zErrMsg;
        
    public:
        int logLevel;
        epoch_histo minutescnt;
        epoch_histo hourscnt;
        epoch_histo dayscnt;
        sqlrw();
        ~sqlrw();
        void create(std::string);
        void open(std::string);
        void close();
        void writeMeta();
        void readMeta();
        void setMin(int, int);
        int getMin(int);
        int getBin(int, const char *, const char *);
        void setBin(int , int , const char * , const char * );
        void readGMHits(std::string);
        sqlite3 * getDB(){return db;};
        void mergeMin();
        unsigned long lastepmilli;
        int runNum;
};

#endif