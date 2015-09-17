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
#include "TROOT.h"
//#include "TRint.h"
#include <time.h>

struct envdata{
    unsigned long epmilli;
    float data;
};

using namespace std;

int make_env_plots(char * dbfname){
    
    int logLevel=0;
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
            if(logLevel) cout << "Opened: " << dbfname << endl;
    } else {
        cout << "Failed to open database: " << dbfname << endl;
        return 1;
    }
    
    int rows = 0;
    sprintf(buff, "SELECT * FROM log ORDER BY epochms DESC LIMIT 144");

    if ( sqlite3_prepare(db, buff, -1, &statement, 0 ) == SQLITE_OK ) {
        int ctotal = sqlite3_column_count(statement);
        int res = 0;
        while ( 1 ){
            res = sqlite3_step(statement);
            if ( res == SQLITE_ROW ) {
                epmilli = stoll( (char*)sqlite3_column_text(statement, 0));
                envbuff = (char*)sqlite3_column_text(statement, 1);
                
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
                        if (logLevel>1) cout << env_muT[i_env_muT].epmilli << " : muT : " << env_muT[i_env_muT].data << "\n";
                        break;
                    case 'C':
                        ++i_env_C;
                        env_C[i_env_C].epmilli=epmilli;
                        env_C[i_env_C].data=stof(data[2]);
                        if(logLevel>1) cout << env_C[i_env_C].epmilli << " : C (" << i_env_C << ") : " << env_C[i_env_C].data << " " << "\n";
                        break;
                    case 'h':
                        ++i_env_hPa;
                        env_hPa[i_env_hPa].epmilli=epmilli;
                        env_hPa[i_env_hPa].data=stof(data[2]);
                        if(logLevel>1) cout << env_hPa[i_env_hPa].epmilli << " : hPa : " << env_hPa[i_env_hPa].data << "\n";
                        break;
                };

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
    if(logLevel==0) gROOT->ProcessLine( "gErrorIgnoreLevel = 1001;");
    gStyle->SetOptStat(0);
    gStyle->SetPadTickY(2);

    float scale=1.5;
    float xoffset=0.53;
    float yoffset=0.15;
    //TCanvas c("c", "c", scale*640,scale*325);    
    TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",scale*640,scale*325);  

    //c1->SetFillColor(42);
    c1->SetGrid();
    
    const Int_t n = 48;
    Double_t x[n], y[n], c[n], p[n];
    unsigned long t0=env_muT[0].epmilli;
    //cout << t0 << endl;
    long diff;
    float hr;
    for (Int_t i=0;i<n;i++) {
      diff=env_muT[n-i-1].epmilli - t0;
      hr=diff/1000/60/60.;
      x[i] = hr; //env_muT[n-i-1].epmilli; //-t0)/3600000.0;
      y[i] = env_muT[n-i-1].data;
      c[i] = env_C[n-i-1].data;
      p[i] = env_hPa[n-i-1].data;
      if(logLevel>1) printf(" i: %i - n-i-1: %i - x[i]: %f - y[i]: %f \n",i, n-i-1,x[i],y[i]);
    }
    unsigned long lastsec = t0 / 1000;
    time_t t = static_cast<time_t>(lastsec);
    TPaveText pt(xoffset,yoffset,xoffset+.34,yoffset+.08,"NBNDC");
    pt.AddText(currentDateTime(t).c_str());      
    
    TGraph *gr = new TGraph(n,x,y);
    TGraph *grC = new TGraph(n,x,c);
    TGraph *grP = new TGraph(n,x,p);
    gr->SetMinimum(10); //10
    gr->SetMaximum(90); //90
    gr->SetLineColor(2);
    gr->SetLineWidth(2);
    gr->SetMarkerColor(4);
    gr->SetMarkerStyle(21);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle("Zaman [Saat]");
    gr->GetYaxis()->SetTitle("Manyetik Alan [#muT]");

    gr->Draw("ACP"); // ACP
    //pt.Draw();    
    // TCanvas::Update() draws the frame, after which one can change it
    c1->Update();
    c1->Print("muT_24h.png");
    
    //c1->Clear();
    //gStyle->SetPadTickY(1);
    grC->SetMinimum(15);
    grC->SetMaximum(35);
    grC->SetLineColor(2);
    grC->SetLineWidth(2);
    grC->SetMarkerColor(8);
    grC->SetMarkerStyle(22);
    grC->SetTitle("");
    grC->GetXaxis()->SetTitle("Zaman [Saat]");
    grC->GetYaxis()->SetTitle("Sicaklik [^{o}C]");
    grC->Draw("ACP");
    //pt.Draw();    
    c1->Print("C_24h.png");
    
    
    grP->SetMinimum(950);
    grP->SetMaximum(1050);
    grP->SetLineColor(3);
    grP->SetLineWidth(2);
    grP->SetMarkerColor(6);
    grP->SetMarkerStyle(23);
    grP->SetTitle("");
    grP->GetXaxis()->SetTitle("Zaman [Saat]");
    grP->GetYaxis()->SetTitle("Atmosfer Basinci [hPa]");
    grP->Draw("ACP");
    //pt.Draw();    
    c1->Update();
    c1->Print("hPa_24h.png");    
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
    return temp;
    
}
