#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include "epoch_histo.h"
#include "mylib.h"

//ROOT References
#include "TFile.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1I.h"

#include <time.h>

using namespace std;

int main(int argc, char **argv){
    string hfile, line; 
    int logLevel=1;
    int mins_since_epoch, count;
    epoch_histo minutescnt;
    sqlite3 *db;
    //hfile="data/histo_00020.dat";
    hfile="data/hist_00020.dat";
    if (argc==2){
        hfile=argv[1];
    }
    cout << "Plotting:" << hfile << endl;

    gStyle->SetOptStat(0);
    gStyle->SetPadTickY(1);
    Int_t font = 8;
    gStyle->SetTextFont(10*font+2);
    TH1I h_hitspm1h("h_hitspm1h","GM Hits per Minute (last hour); Minutes; Hits", 60, -59, 0);

    char buff[100];
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
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) {
        int ctotal = sqlite3_column_count(statement);
        int res = 0;
        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                mins_since_epoch = stoi( (char*)sqlite3_column_text(statement, 0));
                count = stoi( (char*)sqlite3_column_text(statement, 1));
                minutescnt.insert({mins_since_epoch, count});
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

    dump_map(&minutescnt);
    //
    //
    //unsigned int cnthits = 0;
    //
    //unsigned long milliseconds_since_epoch;
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
    //unsigned long lastsec = milliseconds_since_epoch / 1000;
    //myfile.close();
    //time_t t = static_cast<time_t>(lastsec);
    //
    //int lastmin = min, bmin;
    //for (int i=mins.size()-1; i>-1; i--){
    //    bmin=mins.at(i)-lastmin;
    //    if ( bmin < -59 ) break;
    //    //cout << i << ", " << bmin << endl;
    //    h_hitspm1h.Fill(bmin);
    //}
    //
    //float scale=1.5;
    //float xoffset=0.53;
    //float yoffset=0.15;
    //TCanvas c("c", "c", scale*640,scale*480);
    //h_hitspm1h.Draw("E");
    //TPaveText pt(xoffset,yoffset,xoffset+.34,yoffset+.08,"NBNDC");
    //pt.AddText(currentDateTime(t).c_str());    
    //pt.Draw();
    //c.Print("last_hour.png");
    //cout << cnthits << " hits analyzed\n";
    return 0;
}