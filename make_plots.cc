#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//ROOT References
#include "TFile.h"
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



int main(int argc, char **argv){
    string rfile, line;
    rfile="data/gm_00018.txt";
    
    if (argc==2){
        rfile=argv[1];
    }
    cout << "Plotting:" << rfile << endl;
    //return 0;
    gStyle->SetOptStat(0);
    gStyle->SetPadTickY(1);
    Int_t font = 8;
    //gStyle->SetLabelFont(10*font+2);
    gStyle->SetTextFont(10*font+2);
    //gStyle->SetLegendFont(10*font+2);
    //gStyle->SetStatFont(10*font+2);
    //gStyle->SetTitleFont(10*font+2);
    //if (argc < 6){
    //    // Tell the user how to run the program
    //    std::cerr << "Usage: " << argv[0] << " <txt file> <oroot file> <TDirectory> <TTree>" << std::endl;
    //    return 1;
    //}
    //Long64_t nevents = std::atoll(argv[1]);
    //    TH1D h_b1b2Mass("h_b1b2Mass","b1b2Mass; (GeV); Events", 50, 100, 250);
    TH1I h_hitspm1h("h_hitspm1h","GM Hits per Minute (Last Hour); Minutes; Hits", 60, -59, 0);
    //    TH1I h_nVertices("h_nVertices","nVertices; Vertices; Events", 50, 0, 50);,
    //h_hitspm1h.SetLabelFont(10*font+2);
    //h_hitspm1h.SetTitleFont(10*font+2);
    vector<int> mins;
    unsigned int cnthits = 0;
    
    unsigned long milliseconds_since_epoch;
    unsigned long firstmilli=0;
    
    cout << "Opening text file: " << rfile << endl;
    ifstream myfile (rfile);
    if (!myfile.is_open()) return 1; 
    getline (myfile,line);
    cout << line.substr(0,13) << endl;
    milliseconds_since_epoch = stoll(line);
    milliseconds_since_epoch *= 1000;
    firstmilli = milliseconds_since_epoch;
    int min;
    try{
        while ( getline (myfile,line) ){ // 1430 387 170 .830
            ++cnthits;
                //20150430T124610   1430387170.83 561899392
              
              if ( line.size() == 0) break;
              if ( !std::isdigit(line.at(0)) ) break;
              //cout << '"' << line << '"' << endl;
              milliseconds_since_epoch = stoll(line); // !!!!!!!!!! increase length to 13 for new data
              //milliseconds_since_epoch *= 1000;
              min = (milliseconds_since_epoch - firstmilli) / 60000; // convert to min elapsed. float-int conversion truncates.
              //cout << min << endl;
              mins.push_back(min);
        }
    } catch (int param) { cout << "int exception"; }
    cout << "Histogramming\n";
    unsigned long lastsec = milliseconds_since_epoch / 1000;
    myfile.close();
    time_t t = static_cast<time_t>(lastsec);
    
    int lastmin = min, bmin;
    for (int i=mins.size()-1; i>-1; i--){
        bmin=mins.at(i)-lastmin;
        if ( bmin < -59 ) break;
        //cout << i << ", " << bmin << endl;
        h_hitspm1h.Fill(bmin);
    }

    float scale=1.5;
    float xoffset=0.53;
    float yoffset=0.15;
    TCanvas c("c", "c", scale*640,scale*480);
    h_hitspm1h.Draw("E");
    TPaveText pt(xoffset,yoffset,xoffset+.34,yoffset+.08,"NBNDC");
    pt.AddText(currentDateTime(t).c_str());    
    pt.Draw();
    c.Print("last_hour.png");
    cout << cnthits << " hits analyzed\n";
    return 0;
}