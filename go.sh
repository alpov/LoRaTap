#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib64/
gcc sniffer-to-pcap.c -lcjson -lm -o convert
rm -f loratap.pcap
./convert log.txt loratap.pcap
