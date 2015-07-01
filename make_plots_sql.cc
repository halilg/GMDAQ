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

TFile *rootf;
TTree *roott;
int runNum;
ULong64_t millis;

void read_root_file(string fname){    
    rootf = TFile::Open(fname.c_str(),"READ");
    roott = (TTree*) rootf->Get("Meta");
    roott->SetBranchAddress("Run", &runNum);
    roott->SetBranchAddress("LastHitMillis", &millis);
    roott->GetEvent();
    cout << "Run: " << runNum << ", lastmillis: " << millis << endl;
    rootf->Close();
    delete rootf;
}

void create_root_file(string fname){
    runNum=-999;
    millis = 1435281044445;
    rootf = TFile::Open(fname.c_str(),"RECREATE");
    roott = new TTree("Meta","Metadata");
    roott->Branch("Run",&runNum);
    roott->Branch("LastHitMillis",&millis,"LastHitMillis/l");
    roott->Fill();
    rootf->Write();
    rootf->Close();
    delete rootf;
}

int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    //string rfname(argv[0]);
    string rfname("test.root");
    firstmilli = milliseconds_since_epoch;
    if (! fileExists(rfname) ) create_root_file("test.root");
    read_root_file(rfname);
    return 0;
    
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

