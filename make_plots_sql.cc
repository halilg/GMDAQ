#include <stdio.h>
#include <sqlite3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

//ROOT References
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1I.h"

#include "epoch_histo.h"
#include "mylib.h"

using namespace std;

int cnt_hits=0;
vector<int> mins;
unsigned long milliseconds_since_epoch=0;
unsigned long firstmilli=0;
epoch_histo *minutescnt;
epoch_histo *hourscnt;
epoch_histo *dayscnt;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    ++cnt_hits;
    unsigned int min=0;
    for(i=0; i<argc; i++){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if (i==0){ //
            milliseconds_since_epoch = stoll( argv[i] ? argv[i] : "-1" );
        }
        //cout << milliseconds_since_epoch << endl;
    }
    min = (milliseconds_since_epoch - firstmilli) / 60000; // convert to min elapsed. float-int conversion truncates.
    mins.push_back(min);
    if (minutescnt->count(min)) ++(*minutescnt)[min];
    else minutescnt->insert({min, 0});
    return 0;
}

TFile *rootf;
TTree *rootMeta;
TTree *rootData;
//TBranch *brRunNum;
//TBranch *brMillis;
//TBranch *brData;
UInt_t runNum;
ULong64_t millis;

void create_root_file(string fname){
    epoch_histo mhisto;
    runNum=-999;
    millis =-1;
    
    rootf = TFile::Open(fname.c_str(),"RECREATE");
    rootMeta = new TTree("Meta","Metadata");
    rootData = new TTree("Data","Data");
    rootMeta->Branch("Run",&runNum,"runNum/i");
    rootMeta->Branch("LastHitMillis", &millis,"millis/l");
    rootData->Branch("gm",&mhisto);
    
    rootMeta->Fill();
    rootData->Fill();
    
    rootf->Write();
    rootf->Close();
    delete rootf;
    cout << "Root file created: " << fname << endl;
}

void prepare_root_file(string fname){
    rootf = TFile::Open(fname.c_str(),"UPDATE");
    rootMeta = (TTree*) rootf->Get("Meta");
    rootData = (TTree*) rootf->Get("Data");
    rootMeta->SetBranchAddress("Run", &runNum);
    rootMeta->SetBranchAddress("LastHitMillis", &millis);
    rootData->SetBranchAddress("gm",&minutescnt);
    
    rootMeta->GetEvent(0);
    rootData->GetEvent(0);
    cout << "ROOT File opened. Run: " << runNum << ", lastmillis: " << millis << endl;
}

int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    //string rfname(argv[0]);
    string rfname("test.root");
    firstmilli = milliseconds_since_epoch;
    if (! fileExists(rfname) ) create_root_file("test.root");
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
    
    // open ROOT file and set up the tree and branches
    prepare_root_file(rfname);
    
    cout << "Current data in ROOT file:\n";
    dump_map(minutescnt);
    
    // Go
    rc = sqlite3_exec(db, "SELECT * FROM log ORDER BY epochms DESC LIMIT 100", callback, 0, &zErrMsg);
    //rc = sqlite3_exec(db, "SELECT * FROM log ORDER BY epochms DESC", callback, 0, &zErrMsg);

    if( rc!=SQLITE_OK ){
      cout << "SQL error: " << zErrMsg << endl;
      sqlite3_free(zErrMsg);
      rootf->Close();
      delete rootf;
      return 1;
    }
        
    cout << "here\n";
    dump_map(minutescnt);
    
    cout << "minutes: " << mins.size() << endl ;
    int last_minute=mins.at(mins.size()-1);
    //for (int q=0; q<mins.size() ; q++ ) cout << mins.at(q)-last_minute << endl;
    sqlite3_close(db);
    
    cout << "nHits= " << cnt_hits << endl;
    
    
    // Wrap-up
    ++runNum;
    //rootMeta->Fill();
    //rootData->Fill();
    rootf->Write();
    rootf->Close();
    delete rootf;
    cout << "ROOT File closed. Run: " << runNum << ", lastmillis: " << millis << endl;
    
    return 0;
}

