#!/usr/bin/env zsh
ODIR=/home/halil/www/gmlog
WDIR=/home/halil/work/GMDAQ
cd $WDIR
runnum=$(./runs.py -c)
./make_html.py index.html
./make_sql_histos.exe $runnum
./make_plots_sql.exe $runnum
#./make_plots.exe data/gm_00019.txt || exit
mv C_24h.png hPa_24h.png last_hour.png  muT_24h.png index.html $ODIR


