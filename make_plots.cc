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


const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}



int main(int argc, char **argv){
    gStyle->SetOptStat(0);
    //if (argc < 6){
    //    // Tell the user how to run the program
    //    std::cerr << "Usage: " << argv[0] << " <txt file> <oroot file> <TDirectory> <TTree>" << std::endl;
    //    return 1;
    //}
    //Long64_t nevents = std::atoll(argv[1]);
    //    TH1D h_b1b2Mass("h_b1b2Mass","b1b2Mass; (GeV); Events", 50, 100, 250);
    TH1I h_hitspm1h("h_hitspm1h","Hits perMinute in the Last Hour; Minutes; Hits", 60, -59, 0);
    //    TH1I h_nVertices("h_nVertices","nVertices; Vertices; Events", 50, 0, 50);
    vector<int> mins;
    unsigned int cnthits = 0;
    
    unsigned long milliseconds_since_epoch;
    unsigned long firstmilli=0,lastmilli;
    string rfile, line;
    rfile="gm_data/data_00001_28.txt";
    cout << "Opening text file: " << rfile << endl;
    ifstream myfile (rfile);
    if (!myfile.is_open()) return 1;
    getline (myfile,line);
    milliseconds_since_epoch = stoi(line.substr(18,10));
    milliseconds_since_epoch *= 1000;
    firstmilli = milliseconds_since_epoch;
    int min;
    while ( getline (myfile,line) ){ // 1430 387 170 .830
        ++cnthits;
            //20150430T124610   1430387170.83 561899392
          milliseconds_since_epoch = stoi(line.substr(18,10)); // !!!!!!!!!! increase length to 13 for new data
          milliseconds_since_epoch *= 1000;
          min = (milliseconds_since_epoch - firstmilli) / 60000; // convert to min elapsed. float-int conversion truncates.
          //cout << min << endl;
          mins.push_back(min);
    }
    lastmilli = milliseconds_since_epoch;
    myfile.close();
    int lastmin = min, bmin;
    for (int i=mins.size()-1; i>-1; i--){
        bmin=mins.at(i)-lastmin;
        if ( bmin < -59 ) break;
        //cout << i << ", " << bmin << endl;
        h_hitspm1h.Fill(bmin);
    }

    TCanvas c;
    h_hitspm1h.Draw("E");
    TPaveText pt(.5,.5,.95,.8,"brNDC");
    pt.AddText(currentDateTime().c_str());    
    pt.Draw();
    c.Print("last_hour.png");
    cout << cnthits << " hits analyzed\n";
    return 0;
}