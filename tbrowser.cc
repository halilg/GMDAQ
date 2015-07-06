#include <iostream>
#include <string>

//ROOT References
#include "TFile.h"
#include "TBrowser.h"


int main(int argc, char **argv){
   if( argc!=2 ){
      std::cerr << "Usage: " << argv[0] << " <root file>\n";
      return(1);
    }
   TFile f(argv[1]);
   TBrowser b;
}