#!/usr/bin/env zsh
ODIR=/home/halil/www/gmlog
WDIR=/home/halil/work/GMDAQ
cd $WDIR
./make_plots.exe data/gm_00019.txt || exit
mv last_hour.png $ODIR 


