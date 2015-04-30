ifndef ROOTSYS
$(error *** Please set up ROOT)
endif

CPP=g++
CPPFLAGS=-I. `root-config --cflags`
LROOT=`root-config --cflags --glibs`
LOTHER= #-lboost_system

make_plots.exe: make_plots.o Makefile
	$(CPP) make_plots.o $(LROOT) $(LOTHER) -o $@

make_plots.o: make_plots.cc
	$(CPP) -c make_plots.cc $(CPPFLAGS) -o $@

clean:
	rm *.o make_plots.exe