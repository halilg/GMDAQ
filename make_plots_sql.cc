#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <sqlite3.h>
#include "epoch_histo.h"
#include "mylib.h"
#include "sqlrw.h"

//ROOT References
#include "TFile.h"
#include "TPaveText.h"
#include "TStyle.h"
//#include "GStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TFrame.h"
#include "TH1I.h"

#include <time.h>

using namespace std;

int main(int argc, char **argv){
    string hfile, line; 
    int logLevel=0;
    int mins_since_epoch, count;
    epoch_histo minutescnt;
    vector<int> mins;
    sqlite3 *db;

    if( argc!=2 ){
        fprintf(stderr, "Usage: %s <run #>\n", argv[0]);
        return(1);
    }
    
    // Get the run number
    char buff[100];
    int runnum=0;
    try {
        runnum=stoi(argv[1]);
    } catch (std::exception &ex) {
        //printf("M - %s\n", ex.what());
        fprintf(stderr, "Usage: %s <run #>\n", argv[0]);
        return 1;
    }    
    
    sprintf(buff, "data/env_%05d.dat", runnum);
    hfile=buff;
    
    //cout << "Run: " << runnum << endl;
    //return 0;
    //hfile="data/histo_00020.dat";
        // Set the database file names
    sprintf(buff, "data/hist_%05d.dat", runnum);
    hfile=buff;
    
    cout << "Plotting:" << hfile << endl;

    // read metadata
    sqlrw mysqlrw;
    mysqlrw.logLevel=0;
    mysqlrw.open(hfile);
    //dumpTable(mysqlrw.getDB(),"Meta");
    mysqlrw.readMeta();
    //cout << "Read metadata. lastepmilli=" << mysqlrw.lastepmilli << endl;
    mysqlrw.close();    
    
    gStyle->SetOptStat(0);
    gStyle->SetPadTickY(2);
    Int_t font = 8;
    gStyle->SetTextFont(10*font+2);
    TH1I h_hitspm1h("h_hitspm1h","Dakikalik Toplam GM Vuruslari (son bir saat); Dakika; Vurus Sayisi", 60, -59, 0);

    //buff[100]=0;
    sqlite3_stmt *statement;    
    
    if ( sqlite3_open(hfile.c_str(), &db) == SQLITE_OK ){
            ;
    } else {
        cout << "Failed to open database: " << hfile << endl;
        return 1;
    }
    
    int rows = 0;
    sprintf(buff, "SELECT * FROM HistoMin ORDER BY epochmin DESC LIMIT 60");
    //cout << buff << endl;
    mins.clear();
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) {
        int ctotal = sqlite3_column_count(statement);
        int res = 0;
        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                mins_since_epoch = stoi( (char*)sqlite3_column_text(statement, 0));
                count = stoi( (char*)sqlite3_column_text(statement, 1));
                minutescnt.insert({mins_since_epoch, count});
                mins.push_back(count);
                ++rows;
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR) {
                if (logLevel) cout << rows << "Minute Counts read." << endl;
                break;
            }    
        }
    }
    sqlite3_finalize(statement);
    sqlite3_close(db);
    if (logLevel && rows > 0)cout << "Minutes read. Records= " << rows << endl;

    //dump_map(&minutescnt);
    //
    //
    //unsigned int cnthits = 0;
    //
    //unsigned long firstmilli=0;
    //
    //cout << "Opening text file: " << rfile << endl;
    //ifstream myfile (rfile);
    //if (!myfile.is_open()) return 1; 
    //getline (myfile,line);
    //cout << line.substr(0,13) << endl;
    //milliseconds_since_epoch = stoll(line);
    //milliseconds_since_epoch *= 1000;
    //firstmilli = milliseconds_since_epoch;
    //int min;
    //try{
    //    while ( getline (myfile,line) ){ // 1430 387 170 .830
    //        ++cnthits;
    //            //20150430T124610   1430387170.83 561899392
    //          
    //          if ( line.size() == 0) break;
    //          if ( !std::isdigit(line.at(0)) ) break;
    //          //cout << '"' << line << '"' << endl;
    //          milliseconds_since_epoch = stoll(line); // !!!!!!!!!! increase length to 13 for new data
    //          //milliseconds_since_epoch *= 1000;
    //          min = (milliseconds_since_epoch - firstmilli) / 60000; // convert to min elapsed. float-int conversion truncates.
    //          //cout << min << endl;
    //          mins.push_back(min);
    //    }
    //} catch (int param) { cout << "int exception"; }
    //cout << "Histogramming\n";
    unsigned long milliseconds_since_epoch=mysqlrw.lastepmilli;    
    unsigned long lastsec = milliseconds_since_epoch / 1000;
    //myfile.close();
    time_t t = static_cast<time_t>(lastsec);
    //
    //int lastmin = min, bmin;
    for (int i=60; i>0; --i){
        int count=mins.size() > 60-i ? mins.at(60-i) : 0;
        //cout << mins.size() << "(size), i=" << i << " element=" << 60-i << " " << count << endl;        
        h_hitspm1h.SetBinContent(i,count);
        //cout << i << ", " << bmin << endl;
        
    }
    //
    float scale=1.5;
    float xoffset=0.53;
    float yoffset=0.15;
    TCanvas c("c", "c", scale*640,scale*325);
    //h_hitspm1h.SetMaximum()
    h_hitspm1h.SetMinimum(0);
    h_hitspm1h.Draw("E");
    TPaveText pt(xoffset,yoffset,xoffset+.34,yoffset+.08,"NBNDC");
    pt.AddText(currentDateTime(t).c_str());    
    pt.Draw();
    c.Print("last_hour.png");
    //cout << cnthits << " hits analyzed\n";
    return 0;
}
