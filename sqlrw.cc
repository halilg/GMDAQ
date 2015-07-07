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
        static int cbMeta(void *, int, char **, char **);
        static int cbData(void *, int, char **, char **);
        char *zErrMsg;
        
    public:
        epoch_histo minutescnt;
        epoch_histo hourscnt;
        epoch_histo dayscnt;
        static unsigned long lastepmilli;
        static int runNum;
        sqlrw();
        ~sqlrw();
        void create(std::string);
        void open(std::string);
        void close();
        void dump(std::string);
        void write();
        void setMin(int, int);
        int getMin(int);
        void incMin(int);
        void readGMHits(std::string);
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


void sqlrw::setMin(int min, int count){
    sqlite3_stmt *statement;    
    char buff[100];
    int hitcount=-1;
    sprintf(buff, "INSERT INTO HistoMin VALUES (%i,%i)", min, count);
    //cout << buff << endl;
    int res=sqlite3_prepare(db,buff,-1,&statement,0);
    if(res==SQLITE_OK){
        int res=sqlite3_step(statement);
        sqlite3_finalize(statement);
        //cout << "insert OK\n";
    } else {
        ;//cout << res << " : failed\n";
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

int sqlrw::cbMeta(void *NotUsed, int argc, char **argv, char **azColName){
    //cout << (argc == 2) << endl;
    lastepmilli = stoll( argv[0] );
    runNum= stoi( argv[1] );
    return 0;
}

void sqlrw::dump(string c=""){
    zErrMsg = 0;
    int rc = sqlite3_exec(db, "SELECT * FROM Meta LIMIT 1", cbMeta, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
        close();
        return;
    }
    cout << "Lastep=" << lastepmilli << ", run=" << runNum << endl;
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
    
    // These are the initial values for file creation
    lastepmilli=1434467382861;
    runNum=-999;
    write();
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

void sqlrw::write(){
    zErrMsg = 0;
    char buff[100];
    
    int rc = sqlite3_exec(db, "DELETE FROM Meta", cbMeta, 0, &zErrMsg);
    sprintf(buff, "INSERT INTO Meta VALUES ( %lu, %i)", lastepmilli, runNum);
    //std::string buffAsStdStr = buff;
    //string cmd= string() + string(lastepmilli) +  string(", ") + string(runNum) + string(")");
    rc = sqlite3_exec(db, buff, cbMeta, 0, &zErrMsg);
}

void sqlrw::close(){
    sqlite3_close(db);
    db=0;
}

void sqlrw::readGMHits(string fname){
    unsigned long milliseconds_since_epoch=0;
    unsigned int min=0;
    
    bool isOpenDB = false;
    if ( sqlite3_open(fname.c_str(), &db) == SQLITE_OK ){
            isOpenDB = true;
        }         
    sqlite3_stmt *statement;    

    string query("SELECT * FROM log ORDER BY epochms DESC LIMIT 100");

    if ( sqlite3_prepare(db, query.c_str(), -1, &statement, 0 ) == SQLITE_OK ) 
    {
        int ctotal = sqlite3_column_count(statement);
        //cout << "columns: " << ctotal << endl;
        int res = 0;
        --ctotal;

        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                milliseconds_since_epoch = stoll( (char*)sqlite3_column_text(statement, 0));
                min = (milliseconds_since_epoch) / 60000; // convert millis to minutes. float-int conversion truncates.
                if (minutescnt.count(min)) ++minutescnt[min];
                else minutescnt.insert({min, 1});
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR) {
                cout << "done " << endl;
                break;
            }    
        }
        cout << "Current data in memory:\n";
        dump_map(&minutescnt);
    }
    sqlite3_close(db);
}


unsigned long sqlrw::lastepmilli=0;
int sqlrw::runNum=-1;
int main(int argc, char **argv){
    sqlrw mysqlrw;
    //mysqlrw.create("test.dat");
    //mysqlrw.readGMHits("data/gm_00020.dat");
    mysqlrw.open("test.dat");
    cout << "received:" << mysqlrw.getMin(1) << endl;
    mysqlrw.setMin(1,2);
    cout << "received:" << mysqlrw.getMin(1) << endl;
    return 0;
    cout << "Opening DB" << endl;
    
    cout << "dumping \n";
    mysqlrw.dump();
    mysqlrw.close();
    mysqlrw.open("test.dat");
    mysqlrw.lastepmilli = 0;
    mysqlrw.write();
    mysqlrw.close();
    mysqlrw.open("test.dat");
    mysqlrw.dump();
    mysqlrw.close();
    mysqlrw.open("test.dat");
    mysqlrw.runNum = -1;
    mysqlrw.write();
    mysqlrw.close();
    mysqlrw.open("test.dat");
    mysqlrw.dump();
    mysqlrw.setMin(1,10);
    mysqlrw.incMin(1);
    return 0;
}


