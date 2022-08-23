#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib64/
gcc sniffer-to-pcap.c -lcjson -lm -Wall -o convert
#./convert v0 log.txt log_v0.pcap
#./convert v1 log.txt log_v1.pcap
