ifndef ROOTSYS
$(error *** Please set up ROOT)
endif

CPP=g++
CPPFLAGS=-I. `root-config --cflags` -std=c++11
LROOT=`root-config --cflags --glibs`
LOTHER=

ifeq ($(findstring Darwin, $(shell uname)), Darwin)
  LOTHER=-Qunused-arguments #use on mac os to get rid of the warning message: "clang: warning: argument unused during compilation: '-pthread'"
endif

make_plots_sql.exe: make_plots_sql.o Makefile
	$(CPP) make_plots_sql.o $(LROOT) $(LOTHER) -lsqlite3 -o $@

make_plots.exe: make_plots.o Makefile
	$(CPP) make_plots.o $(LROOT) $(LOTHER) -o $@

make_plots_sql.o: make_plots_sql.cc
	$(CPP) -c make_plots_sql.cc $(CPPFLAGS) -o $@

make_plots.o: make_plots.cc
	$(CPP) -c make_plots.cc $(CPPFLAGS) -o $@

clean:
	rm *.o make_plots.exe