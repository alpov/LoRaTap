#!/bin/bash

WORKDIR="../loralog"

#grep -n "listening on port" $WORKDIR/log.txt

mkdir $WORKDIR/log
mkdir $WORKDIR/pcap
rm -f $WORKDIR/pcap/*.pcap

awk 'NR>=628 && NR <= 819 { print }' $WORKDIR/log.txt > $WORKDIR/log/00_Test.log
awk 'NR>=5 && NR <= 147083 { print }' $WORKDIR/log.txt > $WORKDIR/log/01_Brno.log
awk 'NR>=150874 && NR <= 529796 { print }' $WORKDIR/log.txt > $WORKDIR/log/02_Brno.log
awk 'NR>=529800 && NR <= 603895 { print }' $WORKDIR/log.txt > $WORKDIR/log/03_Liege.log
awk 'NR>=603896 { print }' $WORKDIR/log.txt > $WORKDIR/log/04_Liege.log

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib64/

./convert v0 $WORKDIR/log/00_Test.log $WORKDIR/pcap/00_Test_v0.pcap
./convert v1 $WORKDIR/log/00_Test.log $WORKDIR/pcap/00_Test_v1.pcap

for f in $WORKDIR/log/*.log; do
  ./convert v1 "$f" $WORKDIR/pcap/"$(basename "$f" .log)".pcap
done
