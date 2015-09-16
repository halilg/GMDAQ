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

struct envdata{
    unsigned long epmilli;
    float data;
};

using namespace std;

int make_env_plots(char * dbfname){
    
    int logLevel=1;
    sqlite3 *db;
    sqlite3_stmt *statement;    
    char buff[100];
    char * envbuff;
    unsigned long epmilli;
    envdata env_C[100];
    envdata env_muT[100];
    envdata env_hPa[100];
    unsigned int i_env_C=-1;
    unsigned int i_env_muT=-1;
    unsigned int i_env_hPa=-1;
    
    if ( sqlite3_open(dbfname, &db) == SQLITE_OK ){
            cout << "Opened: " << dbfname << endl;
    } else {
        cout << "Failed to open database: " << dbfname << endl;
        return 1;
    }
    
    int rows = 0;
    sprintf(buff, "SELECT * FROM log ORDER BY epochms DESC LIMIT 144");
    cout << buff << endl;
    //return 0;
    //mins.clear();
    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) {
        int ctotal = sqlite3_column_count(statement);
        int res = 0;
        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                epmilli = stoll( (char*)sqlite3_column_text(statement, 0));
                envbuff = (char*)sqlite3_column_text(statement, 1);
                //cout << epmilli << " " << envbuff << endl;
                
                istringstream iss(envbuff);
                string s;
                unsigned int cnt=0;
                string data[3];
                while ( getline( iss, s, ' ' ) ) {
                  data[cnt]=s;
                  ++cnt;
                }
                //cout << epmilli << " : " << data[0] << " " << data[1] << " " << data[2] << "\n";
                switch(data[1].c_str()[0]){
                    case 'm':
                        ++i_env_muT;
                        env_muT[i_env_muT].epmilli=epmilli;
                        env_muT[i_env_muT].data=stof(data[2]);
                        cout << env_muT[i_env_muT].epmilli << " : muT : " << env_muT[i_env_muT].data << "\n";
                        break;
                    case 'C':
                        ++i_env_C;
                        env_C[i_env_C].epmilli=epmilli;
                        env_C[i_env_C].data=stof(data[2]);
                        cout << env_C[i_env_C].epmilli << " : C (" << i_env_C << ") : " << env_C[i_env_C].data << " " << "\n";
                        break;
                    case 'h':
                        ++i_env_hPa;
                        env_hPa[i_env_hPa].epmilli=epmilli;
                        env_hPa[i_env_hPa].data=stof(data[2]);
                        cout << env_hPa[i_env_hPa].epmilli << " : hPa : " << env_hPa[i_env_hPa].data << "\n";
                        break;
                };

                
                
                //count = stoi( (char*)sqlite3_column_text(statement, 1));
                //minutescnt.insert({mins_since_epoch, count});
                //mins.push_back(count);
                ++rows;
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR) {
                if (logLevel) cout << rows << " environment readings." << endl;
                break;
            }    
        }
    }
    sqlite3_finalize(statement);
    sqlite3_close(db);
    
    // All data read to memory
    // Draw the magnetic field graph
    gStyle->SetOptStat(0);
    gStyle->SetPadTickY(2);
    TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);

    //c1->SetFillColor(42);
    c1->SetGrid();
    
    const Int_t n = 48;
    Double_t x[n], y[n], c[n], p[n];
    for (Int_t i=0;i<n;i++) {
      x[i] = double(-i/2);
      y[i] = env_muT[n-i-1].data;
      c[i] = env_C[n-i-1].data;
      p[i] = env_hPa[n-i-1].data;
      printf(" i: %i - n-i-1: %i - x[i]: %i - y[i]: %f \n",i, n-i-1,int(x[i]),y[i]);
    }
    TGraph *gr = new TGraph(n,x,y);
    TGraph *grC = new TGraph(n,x,c);
    TGraph *grP = new TGraph(n,x,p);
    gr->SetMinimum(10);
    gr->SetMaximum(90);
    gr->SetLineColor(2);
    gr->SetLineWidth(2);
    gr->SetMarkerColor(4);
    gr->SetMarkerStyle(21);
    gr->SetTitle("Manyetik alan");
    gr->GetXaxis()->SetTitle("Zaman (Saat)");
    gr->GetYaxis()->SetTitle("B(muT)");
    gr->Draw("ACP");
    
    // TCanvas::Update() draws the frame, after which one can change it
    c1->Update();
    c1->Print("muT_24h.pdf");
    
    //c1->Clear();
    //gStyle->SetPadTickY(1);
    grC->SetMinimum(15);
    grC->SetMaximum(35);
    grC->Draw("ACP");
    c1->Print("C_24h.pdf");
    
    
    grP->SetMinimum(950);
    grP->SetMaximum(1050);
    grP->Draw("ACP");
    c1->Update();
    c1->Print("hPa_24h.pdf");    
    
    
    return 0;
}

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
    int temp=make_env_plots(buff);
    cout << temp << endl;
    return temp;
    
    
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
