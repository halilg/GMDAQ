#!/usr/bin/env zsh

#run test on which run?
trun=1
tdir=test
if [ ! -d $tdir ]; then
   mkdir $tdir
   echo test directory created: $tdir
fi
make all

./make_plots_env.exe $trun
mv C_24h.png hPa_24h.png muT_24h.png $tdir

./make_html.py index.html
./make_sql_histos.exe $trun
./make_plots_sql.exe $trun

mv last_hour.png  index.html $tdir