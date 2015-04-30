#include <iostream>
#include <fstream>
#include <string>
#include "TFile.h"
#include "TString.h"

#include "TTree.h"
#include "TDirectoryFile.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH1I.h"


using namespace std;
int main(int argc, char **argv){
    //if (argc < 6){
    //    // Tell the user how to run the program
    //    std::cerr << "Usage: " << argv[0] << " <txt file> <oroot file> <TDirectory> <TTree>" << std::endl;
    //    return 1;
    //}
    //Long64_t nevents = std::atoll(argv[1]);
    unsigned long milliseconds_since_epoch;
    string rfile, line;
    rfile="gm_data/data_00001_28.txt";
    cout << "Opening text file: " << rfile << endl;
    ifstream myfile (rfile);
    if (myfile.is_open())
      {
        while ( getline (myfile,line) )
        {
            //20150430T124610   1430387170.83 561899392
          //cout << line.substr(18,13) << '\n';
          milliseconds_since_epoch = stoi(line.substr(18,10));
          milliseconds_since_epoch *= 1000;
          //cout << milliseconds_since_epoch << endl;
        }
        myfile.close();
      }

//    TH1D h_b1b2Mass("h_b1b2Mass","b1b2Mass; (GeV); Events", 50, 100, 250);
//    TH1I h_nGoodVertices("h_nGoodVertices","nGoodVertices; Vertices; Events", 50, 0, 50);
//    TH1I h_nVertices("h_nVertices","nVertices; Vertices; Events", 50, 0, 50);

//    cout << "histograms written to: " << orfile << endl;
//    T.Close();
    return 0;
}