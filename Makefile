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

make_plots_sql.exe: make_plots_sql.o epoch_histo.o mylib.o Makefile
	$(CPP) make_plots_sql.o epoch_histo.o mylib.o $(LROOT) $(LOTHER) -lsqlite3 $(FOTHER) -o $@

epoch_histo.o: epoch_histo.cxx
	$(CPP) -c $^ $(CPPFLAGS) -o $@

epoch_histo.cxx: epoch_histo.h
	@echo Generating dictionary..
	rootcint -v -f $@ -c -p $^

make_plots.exe: make_plots.o Makefile
	$(CPP) make_plots.o $(LROOT) $(LOTHER) $(FOTHER) -o $@

make_plots_sql.o: make_plots_sql.cc
	$(CPP) -c make_plots_sql.cc $(CPPFLAGS) -o $@

make_plots.o: make_plots.cc
	$(CPP) -c make_plots.cc $(CPPFLAGS) -o $@

mylib.o: mylib.h mylib.cc 
	$(CPP) -c mylib.cc $(CPPFLAGS) -o $@


clean:
	rm *.o epoch_histo.cxx epoch_histo_rdict.pcm make_plots*.exe