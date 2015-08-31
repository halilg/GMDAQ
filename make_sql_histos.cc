#include <iostream>
//#include <string>
//#include <vector>
#include <cstdio>
//#include "epoch_histo.h"
#include "mylib.h"
#include "sqlrw.h"

void test(){
    ;
    //sqlrw mysqlrw;
    //mysqlrw.open(dbname);
    //cout << "received:" << mysqlrw.getMin(1) << endl;
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //mysqlrw.setMin(1,2);
    //cout << "received:" << mysqlrw.getMin(1) << endl;
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //mysqlrw.setMin(1,30);
    //cout << "received:" << mysqlrw.getMin(1) << endl;
    //dumpTable(mysqlrw.getDB(),"HistoMin");    
}

using namespace std;
int main(int argc, char **argv){
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
    
    // Set the database file names
    sprintf(buff, "data/gm_%05d.dat", runnum);
    string gmdbfile(buff);
    sprintf(buff, "data/hist_%05d.dat", runnum);
    string dbname(buff);
    //cout << gmdbfile << " -> " << dbname << endl;

    sqlrw mysqlrw;
    mysqlrw.logLevel=0;
    
    // if the database file doen't exist, create
    if (! fileExists(dbname) ){
        cout << "Creating histograms database\n";
        mysqlrw.create(dbname);
        mysqlrw.open(dbname);
        mysqlrw.lastepmilli=0;
        mysqlrw.runNum=runnum;
        mysqlrw.writeMeta();
    }
    
    // Read GM data
    mysqlrw.open(dbname);
    //dumpTable(mysqlrw.getDB(),"Meta");
    mysqlrw.readMeta();
    //cout << "Read metadata. lastepmilli=" << mysqlrw.lastepmilli << endl;
    mysqlrw.close();
    mysqlrw.readGMHits(gmdbfile);
    
    mysqlrw.open(dbname);
    mysqlrw.mergeMin();
    cout << "lastepmilli=" << mysqlrw.lastepmilli << endl;
    mysqlrw.writeMeta();
    //cout << "Wrote metadata. lastepmilli=" << mysqlrw.lastepmilli << endl;
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    mysqlrw.close();
    return 0;
    //
    //mysqlrw.open(dbname);
    //dumpTable(mysqlrw.getDB(),"Meta");    
    //mysqlrw.readMeta();
    //mysqlrw.close();
    //mysqlrw.lastepmilli=1434456202948;
    //mysqlrw.runNum=runnum;
    //
    //mysqlrw.writeMeta();
    ////dumpTable(mysqlrw.getDB(),"Meta");
    //mysqlrw.close();
    //mysqlrw.open(dbname);
    //mysqlrw.readMeta();
    //return 0;
    //
    //
    //mysqlrw.readGMHits(gmdbfile);
    //mysqlrw.open(dbname);
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //mysqlrw.mergeMin();
    //dumpTable(mysqlrw.getDB(),"HistoMin");
    //    
    //return 0;
    
}


