ifndef ROOTSYS
	$(error *** Please set up ROOT)
endif

CPP=g++
CPPFLAGS=-I. `root-config --cflags` -std=c++11
LROOT=`root-config --cflags --glibs`
#LOTHER=-L$(shell pwd) -lepoch_histo_h
FOTHER=

ifeq ($(findstring Darwin, $(shell uname)), Darwin)
  FOTHER = -Qunused-arguments #use on mac os to get rid of the warning message: "clang: warning: argument unused during compilation: '-pthread'"
endif

all: make_plots_sql.exe make_sql_histos.exe make_plots_env.exe

make_plots_env.exe: make_plots_env.o mylib.o sqlrw.o 
	$(CPP) make_plots_env.o mylib.o sqlrw.o $(LROOT) $(LOTHER) -lsqlite3 $(FOTHER) -o $@

make_plots_sql.exe: make_plots_sql.o mylib.o sqlrw.o 
	$(CPP) make_plots_sql.o mylib.o sqlrw.o $(LROOT) $(LOTHER) -lsqlite3 $(FOTHER) -o $@

sqlrw.o: sqlrw.cc sqlrw.h
	$(CPP) -c sqlrw.cc $(CPPFLAGS) $(FOTHER) -o $@

make_sql_histos.exe: make_sql_histos.o mylib.o sqlrw.o
	$(CPP) $^ $(LROOT) $(LOTHER) $(FOTHER) -lsqlite3 -o $@

make_sql_histos.o: make_sql_histos.cc
	$(CPP) -c $^ $(CPPFLAGS) $(FOTHER) -o $@

tbrowser.exe: tbrowser.cc
	$(CPP) $^ $(CPPFLAGS) $(LROOT) $(LOTHER) -lsqlite3 $(FOTHER) -o $@
	
epoch_histo.o: epoch_histo.cxx
	$(CPP) -c $^ $(CPPFLAGS) -o $@

epoch_histo.cxx: epoch_histo.h
	@echo Generating dictionary..
	rootcint -v -f $@ -c -p $^

make_plots_sql.o: make_plots_sql.cc
	$(CPP) -c make_plots_sql.cc $(CPPFLAGS) -o $@

make_plots_env.o: make_plots_env.cc
	$(CPP) -c make_plots_env.cc $(CPPFLAGS) -o $@

mylib.o: mylib.h mylib.cc 
	$(CPP) -c mylib.cc $(CPPFLAGS) -o $@

clean:
	rm -f *.o epoch_histo.cxx epoch_histo_rdict.pcm make_plots*.exe