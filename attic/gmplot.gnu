set terminal canvas size 1400,400 enhanced font 'Verdana,10'

set title "GM Hits"
set xlabel "Time"
set ylabel "Hits"
set xdata time
set timefmt "%H:%M"
set format x "%H:%M"

plot 'hits.txt' using 1:2 with linespoints ls 1 
