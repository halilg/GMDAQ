#include <sqlite3.h>
#include <iostream>


class sqlrw {
    private:
        sqlite3 *db;
        bool fileopened;
    public:
        unsigned long lastepmilli;
        int runNum;
        sqlrw();
        ~sqlrw();
        void create(std::string);
        void open(std::string);
        void close();
        void dump(std::string);
        void write();
};


     


rootrw::rootrw(){
    runNum=0;
    lastepmilli=1434467382861;
    fileopened=false;
}

rootrw::~rootrw(){
    ;
}

void rootrw::dump(string c){
    ;
}

void rootrw::create(string fname){
    open(fname);
    rc = sqlite3_exec(db, "SELECT * FROM log ORDER BY epochms DESC LIMIT 100", callback, 0, &zErrMsg);
           ("CREATE TABLE if not exists Meta (epochms INTEGER, runNum INTEGER)")
    //rc = sqlite3_exec(db, "SELECT * FROM log ORDER BY epochms DESC", callback, 0, &zErrMsg);

    if( rc!=SQLITE_OK ){
      cout << "SQL error: " << zErrMsg << endl;
      sqlite3_free(zErrMsg);
      rootf->Close();
      delete rootf;
      return 1;
    }
    ;
}

void rootrw::open(string fname){
    
    int rc = sqlite3_open(fname, &db);
    if( rc ){
      cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
      sqlite3_close(db);
      return;
    }
    fileopened=true;
}

void rootrw::write(){
    ;
}

void rootrw::close(){
    ;
}



int main(int argc, char **argv){
    return 0;
}


