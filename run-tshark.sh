#!/bin/bash

WORKDIR=".."

CSV_FORMAT="-T fields -E separator=,
    -e frame.number -e frame.time_epoch -e frame.len -e loratap.srcgw -e loratap.flags.crc
    -e loratap.rssi.current -e loratap.rssi.snr -e loratap.channel.frequency -e loratap.channel.sf -e loratap.channel.cr
    -e lorawan.mhdr.ftype"

FLT_LORAWAN_VALID="(((loratap.flags.crc == 0x01) || ((loratap.flags.crc == 0x04) && ((lorawan.mhdr.ftype == 3) ||
    lorawan.mhdr.ftype == 5))) && !(lorawan.mhdr_error))"

FLT_BEACON_ALL="lorawan.msgtype == \"Class-B Beacon\""

FLT_BEACON_VALID="(lorawan.msgtype == \"Class-B Beacon\") && (lorawan.beacon.crc1.status == \"Good\") && 
    (lorawan.beacon.crc2.status == \"Good\")"

mkdir $WORKDIR/csv
rm -f $WORKDIR/csv/*.csv

for f in $WORKDIR/pcap/*.pcap; do
  DATASET="$(basename "$f" .pcap)"

  tshark -r "$f" $CSV_FORMAT > $WORKDIR/csv/${DATASET}_all.csv
  printf "%s,TOTAL,%d\n" $DATASET `wc -l < $WORKDIR/csv/${DATASET}_all.csv`

  tshark -r "$f" $CSV_FORMAT -Y "$FLT_LORAWAN_VALID" > $WORKDIR/csv/${DATASET}_lorawan.csv
  printf "%s,LORAWAN,%d\n" $DATASET `wc -l < $WORKDIR/csv/${DATASET}_lorawan.csv`

  tshark -r "$f" $CSV_FORMAT -Y "$FLT_BEACON_VALID" > $WORKDIR/csv/${DATASET}_beacon.csv
  printf "%s,BEACON,%d\n" $DATASET `wc -l < $WORKDIR/csv/${DATASET}_beacon.csv`
done
