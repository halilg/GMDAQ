#!/usr/bin/env zsh
MYLOC=$(cd "$(dirname "$0")"; pwd)
WWWLOC=/home/halil/www/gmlog

test -e $MYLOC/maintenance.is || (/usr/bin/nice -n 20 $MYLOC/publish_plots.zsh > /dev/null 2>&1 )
test -e $MYLOC/maintenance.is && (cp $MYLOC/maintenance.png $WWWLOC/last_hour.png)
