#!/usr/bin/env zsh
ODIR=/home/halil/www/gmlog
WDIR=/home/halil/work/GMDAQ
cd $WDIR
runnum=$(./runs.py -c)
./make_html.py index.html
./make_sql_histos.exe $runnum
./make_plots_sql.exe $runnum
#./make_plots.exe data/gm_00019.txt || exit
mv last_hour.png  index.html $ODIR
[[ -e  C_24h.png ]] && mv C_24h.png $ODIR
[[ -e hPa_24h.png ]] && mv hPa_24h.png $ODIR
[[ -e muT_24h.png ]] && mv muT_24h.png $ODIR

