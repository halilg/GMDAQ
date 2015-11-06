#!/usr/bin/env zsh

ODIR=/home/halil/www/gmlog
SDIR=$(cd "$(dirname "$0")"; pwd)

rm $ODIR/*.png
rm -f $SDIR/*.pid*
rm -f $SDIR/foton.MainThread*
$SDIR/gmdaq.py start
