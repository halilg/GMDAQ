// !! doesn't work

#include <iostream>
#include <string>

//ROOT References
#include "TFile.h"
#include "TBrowser.h"


using namespace std;
int main(int argc, char **argv){
    if( argc!=2 ){
        std::cerr << "Usage: " << argv[0] << " <root file>\n";
        return(1);
    }
    cout << "Opening " << argv[1] << endl;
    TFile f(argv[1]);
    cout << "done\n";
    TBrowser b("b","ROOT Browser");
}