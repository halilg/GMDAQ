#include <iostream>
#include <string>

//ROOT References
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1I.h"

class rootrw {
    TFile *rootf;
    TTree *rootMeta;
    TTree *rootData;
    UInt_t runNum;
    ULong64_t millis;
    
  public:
    void create(std::string);
    void open(std::string);
    void close();
};


using namespace std;
void rootrw::create(string fname){
    runNum=-999;
    millis =-1;
    
    rootf = TFile::Open(fname.c_str(),"RECREATE");
    rootMeta = new TTree("Meta","Metadata");
    rootMeta->Branch("Run",&runNum,"runNum/i");
    rootMeta->Branch("LastHitMillis", &millis,"millis/l");
    
    rootMeta->Fill();
    rootData->Fill();
    
    rootf->Write();
    rootf->Close();
    delete rootf;
    cout << "Root file created: " << fname << endl;
}

void rootrw::open(string fname){
    rootf = TFile::Open(fname.c_str(),"UPDATE");
    rootMeta = (TTree*) rootf->Get("Meta");
    rootMeta->SetBranchAddress("Run", &runNum);
    rootMeta->SetBranchAddress("LastHitMillis", &millis);
    cout << "ROOT File opened. Run: " << runNum << ", lastmillis: " << millis << endl;
}

void rootrw::close(){
    rootf->Write();
    rootf->Close();
    delete rootf;            
}


int main(int argc, char **argv){
    rootrw myroot;
    myroot.create("test.root");
    return 0;
}

