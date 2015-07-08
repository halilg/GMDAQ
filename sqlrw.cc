#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include "epoch_histo.h"
#include "mylib.h"

//https://www.sqlite.org/cintro.html
//http://www.codeproject.com/Tips/378808/Accessing-a-SQLite-Database-with-Cplusplus

//sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
// Any (modifying) SQL commands executed here are not committed until at the you call:
//sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
//std::vector<int> mins;
class sqlrw {
    private:
        sqlite3 *db;
        bool fileopened;
        static int cbData(void *, int, char **, char **);
        static int cbMeta(void *, int , char **, char **);

        char *zErrMsg;
        
        
    public:
        epoch_histo minutescnt;
        epoch_histo hourscnt;
        epoch_histo dayscnt;
        //static unsigned long lastepmilli;
        //static int runNum;
        sqlrw();
        ~sqlrw();
        void create(std::string);
        void open(std::string);
        void close();
        void writeMeta();
        void readMeta();
        void setMin(int, int);
        int getMin(int);
        void incMin(int);
        void readGMHits(std::string);
        sqlite3 * getDB(){return db;};
        void mergeMin();
        unsigned long lastepmilli;
        int runNum;
};

sqlrw::sqlrw(){
    runNum=0;
    lastepmilli=-1;
    fileopened=false;
}

sqlrw::~sqlrw(){
    sqlite3_close(db);
}

using namespace std;

void sqlrw::mergeMin(){
    //dump_map(&minutescnt);
    int min, cnt;
    cout << "Merging\n";
    for( epoch_histo::iterator ii=minutescnt.begin(); ii!=minutescnt.end(); ++ii){
        min=(*ii).first;
        cnt=(*ii).second;
        std::cout << min << ": " << cnt << std::endl;
        if (getMin(min) == -1){
            setMin(min,getMin(min)+cnt+1); // can to be optimized, row is checked twice
        } else {
            setMin(min,getMin(min)+cnt); // can to be optimized, row is checked twice
        }
        
    }
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
    //cout << buff << endl;
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
        //cout << "columns: " << ctotal << endl;
        int res = 0;

        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                hitcount = stoi( (char*)sqlite3_column_text(statement, 1));
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR) {
                //cout << "done " << endl;
                break;
            }    
        }
    }    
    return hitcount;
}

void sqlrw::incMin(int){
    ;
}

void sqlrw::create(string fname){
    open(fname);
    zErrMsg = 0;
    int rc = sqlite3_exec(db, "CREATE TABLE if not exists Meta (epochms INTEGER, runNum INTEGER)", cbMeta, 0, &zErrMsg);
    rc = sqlite3_exec(db, "CREATE TABLE if not exists HistoMin (epochmin INTEGER, count INTEGER)", cbMeta, 0, &zErrMsg);

    if( rc!=SQLITE_OK ){
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
        close();
        return;
    }
    cout << "Histogram database created: " << fname << endl;
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

int sqlrw::cbMeta(void *NotUsed, int argc, char **argv, char **azColName){
//    //cout << (argc == 2) << endl;
//    lastepmilli = stoll( argv[0] );
//    runNum= stoi( argv[1] );
    return 0;
}

void sqlrw::writeMeta(){
    sqlite3_stmt *statement;    
    char buff[100];
    zErrMsg=0;
    //sprintf(buff, "");
    sqlite3_exec(db,"DELETE FROM Meta",NULL,NULL,&zErrMsg);
    //if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) int res = sqlite3_step(statement);
    //else {
    //std::cout << sqlite3_errmsg(db) << " (" << sqlite3_errstr(sqlite3_extended_errcode(db)) << ")" << std::endl;
    //}
    sprintf(buff, "INSERT INTO Meta VALUES (%lu,%i)", lastepmilli, runNum);
    //cout << buff << endl;
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ){
        int res = sqlite3_step(statement);
        sqlite3_finalize(statement);
        //cout << "write meta OK " << res << endl;
    }
    else {
        std::cout << sqlite3_errmsg(db) << " (" << sqlite3_errstr(sqlite3_extended_errcode(db)) << ")" << std::endl;
    }
    //cout << res << endl;
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
            cout << "lastepp=" << sqlite3_column_text(statement, 0) << ", runNum=" << sqlite3_column_text(statement, 1) << endl;
        }
            
    }
}

void sqlrw::close(){
    sqlite3_close(db);
    db=0;
}

void sqlrw::readGMHits(string fname){
    unsigned long milliseconds_since_epoch=0;
    unsigned int min=0;
    char buff[100];

    
    bool isOpenDB = false;
    if ( sqlite3_open(fname.c_str(), &db) == SQLITE_OK ){
            isOpenDB = true;
        }         
    sqlite3_stmt *statement;    

    
    sprintf(buff, "SELECT * FROM log WHERE epochms>%lu ORDER BY epochms ASC LIMIT 100", lastepmilli);

    cout << buff << endl;
    //epochms
    
    //string query("SELECT * FROM log ORDER BY epochms ASC LIMIT 100"); // LIMIT 100");

    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) 
    {
        int ctotal = sqlite3_column_count(statement);
        int rows = 0;
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
                cout << rows << " GM hits read." << endl;
                break;
            }    
        }
    }
    sqlite3_close(db);
    cout << "GM data read. lastepmilli=" << lastepmilli << endl;
}


void test(){
    ;
    //sqlrw mysqlrw;
    //mysqlrw.open(dbname);
    //cout << "received:" << mysqlrw.getMin(1) << endl;
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //mysqlrw.setMin(1,2);
    //cout << "received:" << mysqlrw.getMin(1) << endl;
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //mysqlrw.setMin(1,30);
    //cout << "received:" << mysqlrw.getMin(1) << endl;
    //dumpTable(mysqlrw.getDB(),"HistoMin");    
}


int main(int argc, char **argv){
    if( argc!=2 ){
        fprintf(stderr, "Usage: %s <run #>\n", argv[0]);
        return(1);
    }

    char buff[100];
    try {
    sprintf(buff, "data/gm_%05d.dat", stoi(argv[1]));
    } catch (std::exception &ex) {
        //printf("M - %s\n", ex.what());
        fprintf(stderr, "Usage: %s <run #>\n", argv[0]);
        return 1;
    }
    string gmdbfile(buff);
    int runnum=stoi(argv[1]);
    sprintf(buff, "data/hist_%05d.dat", runnum);
    string dbname(buff);
    cout << gmdbfile << " -> " << dbname << endl;
    
    sqlrw mysqlrw;
    if (! fileExists(dbname) ){ // create database
        mysqlrw.create(dbname);
        mysqlrw.open(dbname);
        mysqlrw.lastepmilli=0;
        mysqlrw.runNum=runnum;
        mysqlrw.writeMeta();
    }
    //mysqlrw.open(dbname);
    //dumpTable(mysqlrw.getDB(),"Meta");    
    //mysqlrw.readMeta();
    //mysqlrw.close();
    mysqlrw.readGMHits(gmdbfile);
    
    mysqlrw.open(dbname);
    //dumpTable(mysqlrw.getDB(),"Meta");
    mysqlrw.readMeta();
    cout << "Read meta. lastepmilli=" << mysqlrw.lastepmilli << endl;
    
    mysqlrw.mergeMin();
    cout << "lastepmilli=" << mysqlrw.lastepmilli << endl;
    mysqlrw.writeMeta();
    mysqlrw.close();
    return 0;
    //
    //mysqlrw.lastepmilli=1434456202948;
    //mysqlrw.runNum=runnum;
    //
    //mysqlrw.writeMeta();
    ////dumpTable(mysqlrw.getDB(),"Meta");
    //mysqlrw.close();
    //mysqlrw.open(dbname);
    //mysqlrw.readMeta();
    //return 0;
    //
    //
    //mysqlrw.readGMHits(gmdbfile);
    //mysqlrw.open(dbname);
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //mysqlrw.mergeMin();
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //    
    //return 0;
    
}


