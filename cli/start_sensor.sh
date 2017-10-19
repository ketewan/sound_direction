#!/bin/sh -x

SNDFIFO=/tmp/sound.fifo
EXE=cli-trik

rm -f $SNDFIFO
mkfifo $SNDFIFO
#killall cat # Argh...
cat $SNDFIFO > /dev/null & 
READER=$!
killall $EXE 2>/dev/null || echo "No $EXE is running, that's fine"
/home/root/trik/$EXE -V -s v -T 50000000 --diff-time 15 \
   < /dev/null > $SNDFIFO 
rm -f $SNDFIFO
kill -9 $READER || echo "No background reader was runnig, that's strange"
