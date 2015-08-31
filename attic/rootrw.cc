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

#include "epoch_histo.h"
#include "mylib.h"

typedef struct {
    double millis;
    int runNum;
   } GMMETA;

class rootrw {
    private:
        TFile *rootf;
        TTree *rootMeta;
    public:
        GMMETA meta;
        epoch_histo *minutescnt;
        epoch_histo *hourscnt;
        epoch_histo *dayscnt;
        rootrw();
        ~rootrw();
        void create(std::string);
        void open(std::string);
        void close();
        void dump(std::string);
        void write();
};

using namespace std;

rootrw::rootrw(){
    meta.runNum=0;
    meta.millis=1434467382861;
    rootf=0;
    dump("before fill");
}

rootrw::~rootrw(){
    if (rootf) rootf->Close();
}

void rootrw::dump(string c){
    printf ("%s : Run: %6ld, Lastmillis: %13llu\n", c.c_str(), static_cast<long>(meta.runNum), static_cast<unsigned long long>(meta.millis));
    if (minutescnt) dump_map(minutescnt);
    else cout << "Histogram N/A\n";
}

void rootrw::create(string fname){
    meta.runNum=-999;
    epoch_histo mhisto;
    mhisto[1]=1;
    mhisto[2]=2;
    //meta.millis =-1;
    dump("before branch");
    rootf = TFile::Open(fname.c_str(),"RECREATE");
    rootMeta = new TTree("Meta","Meta");
    rootMeta->Branch("runNum",&meta.runNum);
    rootMeta->Branch("millis",&meta.millis);
    rootMeta->Branch("mhisto",&mhisto);
    //cout << "here1\n";
    meta.runNum=-999;
    //meta.millis =-1;
    dump("->");
    rootMeta->Fill();
    rootf->Write();
    rootf->Close();
    delete rootf;
    cout << "ROOT File created:" << fname << endl;
    dump("->");
}

void rootrw::open(string fname){
    meta.runNum=0;
    meta.millis =0;    
    rootf = TFile::Open(fname.c_str(),"UPDATE");
    rootMeta = (TTree*) rootf->Get("Meta");
    rootMeta->SetBranchAddress("runNum", &meta.runNum);
    rootMeta->SetBranchAddress("millis", &meta.millis);
    rootMeta->SetBranchAddress("mhisto", &minutescnt);
    rootMeta->GetEvent(0);
    cout << "ROOT File opened:" << fname << endl;
}

void rootrw::write(){
    rootf->Write();
}

void rootrw::close(){
    rootf->Write();
    rootf->Close();
    delete rootf;
    rootf=0;
}


int main(int argc, char **argv){
    // test
    rootrw myroot;
    myroot.create("test.root");
    myroot.dump("lol0");
    myroot.open("test.root");
    myroot.dump("lol1");
    myroot.meta.runNum=1;
    myroot.close();
    myroot.open("test.root");
    myroot.dump("lol2");
    
    myroot.close();
    return 0;
}

