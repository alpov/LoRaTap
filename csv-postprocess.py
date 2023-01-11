#!/usr/bin/python3

import csv, sys

fi = open(sys.argv[1], 'r')
fo = open(sys.argv[2], 'w')
writer = csv.writer(fo, delimiter=',')

for line in csv.reader(fi):
    line[2] = int(line[2]) - 35

    if line[3] != '':
        line[3] = line[3][len(line[3])-1]

    if line[4] != '':
        line[4] = int(line[4], 16)

    line[5] = -139 + float(line[5])

    snr = int(line[6])
    if snr > 127: snr = snr - 256
    line[6] = snr / 4

    #print(line)
    writer.writerow(line)
