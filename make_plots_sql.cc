#include <stdio.h>
#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//ROOT References
#include "TFile.h"
#include "TTree.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1I.h"

#include <time.h>


using namespace std;

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

int cnt_hits=0;
vector<int> mins;
unsigned long milliseconds_since_epoch=0;
unsigned long firstmilli=0;
    
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    ++cnt_hits;
    unsigned int min=0;
    for(i=0; i<argc; i++){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if (i==0){ //
            milliseconds_since_epoch = stoll( argv[i] ? argv[i] : "NULL" );
        }
        //cout << milliseconds_since_epoch << endl;
    }
    min = (milliseconds_since_epoch - firstmilli) / 60000; // convert to min elapsed. float-int conversion truncates.
    mins.push_back(min);
    //printf("\n");
    return 0;
}


TFile *f;  
int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    firstmilli = milliseconds_since_epoch;
  
    if( argc!=2 ){
      fprintf(stderr, "Usage: %s <database file>\n", argv[0]);
      return(1);
    }
    rc = sqlite3_open(argv[1], &db);
    if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
    }
    
    rc = sqlite3_exec(db, "SELECT * FROM log ORDER BY epochms DESC LIMIT 4000", callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    
    cout << "minutes: " << mins.size() << endl ;
    int last_minute=mins.at(mins.size()-1);
    for (int q=0; q<mins.size() ; q++ ) cout << mins.at(q)-last_minute << endl;
    sqlite3_close(db);
    cout << "nHits= " << cnt_hits << endl;
    return 0;
}

