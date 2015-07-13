#include "sqlrw.h"
#include <iostream>
#include <vector>
#include "mylib.h"

//https://www.sqlite.org/cintro.html
//http://www.codeproject.com/Tips/378808/Accessing-a-SQLite-Database-with-Cplusplus

//sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
// Any (modifying) SQL commands executed here are not committed until at the you call:
//sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
//std::vector<int> mins;




sqlrw::sqlrw(){
    runNum=0;
    lastepmilli=-1;
    fileopened=false;
    logLevel=0;
}

sqlrw::~sqlrw(){
    sqlite3_close(db);
}

using namespace std;

void sqlrw::mergeMin(){
    if (minutescnt.size() == 0){
        if (logLevel) cout <<"No data to merge\n";
        return;
    }
    
    int min, cnt;
    if (logLevel) cout << "Merging\n";
    zErrMsg=0;
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, 0, &zErrMsg);
    if (zErrMsg) cout << "mergeMin failed (0): " << zErrMsg << endl;
    for( epoch_histo::iterator ii=minutescnt.begin(); ii!=minutescnt.end(); ++ii){
        min=(*ii).first;
        cnt=(*ii).second;
        if (logLevel>1) std::cout << min << ": " << cnt << std::endl;
        if (getMin(min) == -1){
            setMin(min,getMin(min)+cnt+1); // can to be optimized, row is checked twice
        } else {
            setMin(min,getMin(min)+cnt); // can to be optimized, row is checked twice
        }
        
    }
    zErrMsg=0;
    rc = sqlite3_exec(db, "END TRANSACTION", NULL, 0, &zErrMsg);
    if (zErrMsg) cout << "mergeMin failed (1): " << zErrMsg << endl;
}

void sqlrw::setMin(int min, int count){
    sqlite3_stmt *statement;    
    int hitcount=-1;
    char buff[100];
    
    if (getMin(min) == -1) { // new record
        sprintf(buff, "INSERT INTO HistoMin VALUES (%i,%i)", min, count);
    } else { // update
        sprintf(buff, "UPDATE HistoMin set count=%i where epochmin=%i", count, min);
    }
    int res=sqlite3_prepare(db,buff,-1,&statement,0);
    if(res==SQLITE_OK){
        int res=sqlite3_step(statement);
        sqlite3_finalize(statement);
    } 
}

int sqlrw::getMin(int min){
    sqlite3_stmt *statement;    
    char buff[100];
    int hitcount=-1;
    sprintf(buff, "SELECT * FROM HistoMin WHERE epochmin=%i", min);
    
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) {
        int ctotal = sqlite3_column_count(statement);
        int res = 0;

        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                hitcount = stoi( (char*)sqlite3_column_text(statement, 1));
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR) {
                break;
            }    
        }
    }    
    return hitcount;
}

void sqlrw::create(string fname){
    open(fname);
    zErrMsg = 0;
    int rc = sqlite3_exec(db, "CREATE TABLE if not exists Meta (epochms INTEGER, runNum INTEGER)", NULL, 0, &zErrMsg);
    rc = sqlite3_exec(db, "CREATE TABLE if not exists HistoMin (epochmin INTEGER, count INTEGER)", NULL, 0, &zErrMsg);

    if( rc!=SQLITE_OK ){
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
        close();
        return;
    }
    if (logLevel) cout << "Histogram database created: " << fname << endl;
    close();
}

void sqlrw::open(string fname){
    
    int rc = sqlite3_open(fname.c_str(), &db);
    if( rc ){
      cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
      close();
      return;
    }
    fileopened=true;
}

void sqlrw::writeMeta(){
    sqlite3_stmt *statement;    
    char buff[100];
    zErrMsg=0;
    sqlite3_exec(db,"BEGIN TRANSACTION",NULL,NULL,&zErrMsg);
    sqlite3_exec(db,"DELETE FROM Meta",NULL,NULL,&zErrMsg);
    if (zErrMsg) cout << "writeMeta failed: " << zErrMsg << endl;
    sprintf(buff, "INSERT INTO Meta VALUES (%lu,%i)", lastepmilli, runNum);
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ){
        int res = sqlite3_step(statement);
        sqlite3_finalize(statement);
    }
    else {
        std::cout << sqlite3_errmsg(db) << " (" << sqlite3_errstr(sqlite3_extended_errcode(db)) << ")" << std::endl;
    }
    sqlite3_exec(db,"END TRANSACTION",NULL,NULL,&zErrMsg);
}

void sqlrw::readMeta(){
    sqlite3_stmt *statement;    
    char buff[100];
    sprintf(buff, "SELECT * FROM Meta LIMIT 1 ");
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) 
    {
        int res = sqlite3_step(statement);
        if ( res == SQLITE_ROW ) {
            lastepmilli = stoll( (char*)sqlite3_column_text(statement, 0));
            runNum = stoi( (char*)sqlite3_column_text(statement, 1));
        }
            
    }
    sqlite3_finalize(statement);
}

void sqlrw::close(){
    sqlite3_close(db);
    db=0;
}

void sqlrw::readGMHits(string fname){
    unsigned long milliseconds_since_epoch=0;
    unsigned int min=0;
    char buff[100];
    sqlite3_stmt *statement;    
    
    bool isOpenDB = false;
    if ( sqlite3_open(fname.c_str(), &db) == SQLITE_OK ){
            isOpenDB = true;
    } else {
        cout << "Failed to open database: " << fname << endl;
        return;
    }
    
    int rows = 0;
    //string limit("LIMIT 100");
    string limit("");
    sprintf(buff, "SELECT * FROM log WHERE epochms>%lu ORDER BY epochms ASC %s", lastepmilli, limit.c_str());
    //cout << buff << endl;
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) {
        int ctotal = sqlite3_column_count(statement);
        int res = 0;
        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                milliseconds_since_epoch = stoll( (char*)sqlite3_column_text(statement, 0));
                lastepmilli = milliseconds_since_epoch;
                min = (milliseconds_since_epoch) / 60000; // convert millis to minutes. float-int conversion truncates.
                if (minutescnt.count(min)) ++minutescnt[min];
                else minutescnt.insert({min, 1});
                ++rows;
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR) {
                if (logLevel) cout << rows << " GM hits read." << endl;
                break;
            }    
        }
    }
    sqlite3_finalize(statement);
    sqlite3_close(db);
    if (logLevel && rows > 0)cout << "GM data read. Records= " << rows << ", lastepmilli=" << lastepmilli << endl;
}