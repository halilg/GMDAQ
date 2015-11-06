#!/usr/bin/env zsh
MYLOC=$(cd "$(dirname "$0")"; pwd)
echo $ABSPATH
exit
test -e $MYLOC/maintenance.is || (/usr/bin/nice -n 20 $MYLOC/publish_plots.zsh -e  > /dev/null 2>&1 )
