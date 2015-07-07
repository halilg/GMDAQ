#include <sqlite3.h>
#include <iostream>
#include <string>
#include <cstdio>

//https://www.sqlite.org/cintro.html
//http://www.codeproject.com/Tips/378808/Accessing-a-SQLite-Database-with-Cplusplus

//sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
// Any (modifying) SQL commands executed here are not committed until at the you call:
//sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

class sqlrw {
    private:
        sqlite3 *db;
        bool fileopened;
        static int cbMeta(void *, int, char **, char **);
        static int cbData(void *, int, char **, char **);
        
    public:
        static unsigned long lastepmilli;
        static int runNum;
        sqlrw();
        ~sqlrw();
        void create(std::string);
        void open(std::string);
        void close();
        void dump(std::string);
        void write();
        void resetMin(int);
        void setMin(int, int);
        int getMin(int);
        void incMin(int);
};

sqlrw::sqlrw(){
    runNum=0;
    lastepmilli=-1;
    fileopened=false;
}

sqlrw::~sqlrw(){
    sqlite3_stmt *statement;    
}

using namespace std;


int sqlrw::cbData(void *NotUsed, int argc, char **argv, char **azColName){
    //cout << (argc == 2) << endl;
    //lastepmilli = stoll( argv[0] );
    //runNum= stoi( argv[1] );
    return 0;
}

void sqlrw::resetMin(int){
    char buff[100];    
    int rc = sqlite3_exec(db, "DELETE FROM Meta", cbMeta, 0, &zErrMsg);
    sprintf(buff, "INSERT INTO Meta VALUES ( %lu, %i)", lastepmilli, runNum);
    
    int rc = sqlite3_exec(db, "SELECT * FROM Meta LIMIT 1", cbMeta, 0, &zErrMsg);
}

void sqlrw::setMin(int, int){
    ;
}

int sqlrw::getMin(int){
    return 0;
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
    char *zErrMsg = 0;
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
    char *zErrMsg = 0;
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
    char *zErrMsg = 0;
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


unsigned long sqlrw::lastepmilli=0;
int sqlrw::runNum=-1;
int main(int argc, char **argv){
    sqlrw mysqlrw;
    mysqlrw.create("test.dat");
    cout << "Opening DB" << endl;
    mysqlrw.open("test.dat");
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
    mysqlrw.resetMin(1);
    mysqlrw.setMin(1,10);
    mysqlrw.incMin(1);
    return 0;
}


