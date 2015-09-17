#!/usr/bin/env zsh
ODIR=/home/halil/www/gmlog
WDIR=/home/halil/work/GMDAQ
cd $WDIR
runnum=$(./runs.py -c)
if [[ $# -eq 1 ]]; then
   if [[ $1 == "-e" ]]; then
       echo creating environment plots
       ./make_plots_env.exe $runnum
       mv C_24h.png hPa_24h.png muT_24h.png $ODIR
   fi
fi

./make_html.py index.html
./make_sql_histos.exe $runnum
./make_plots_sql.exe $runnum
#./make_plots.exe data/gm_00019.txt || exit
mv last_hour.png  index.html $ODIR

