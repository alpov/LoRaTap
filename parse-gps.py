import csv, sys
from datetime import datetime

filename = sys.argv[1]  # replace with your CSV file
output_file = sys.argv[2]

# Wireshark beacon GPS filter
# ((lorawan.msgtype == "Class-B Beacon") && (lorawan.beacon.crc2.status == "Good")) && (lorawan.beacon.crc2 == 0x0000)

unique_gps = {}

with open(filename, 'r') as csvfile:
    reader = csv.reader(csvfile)
    next(reader)  # skip header row
    for row in reader:
        latitude = row[13].replace(',', '.')
        longitude = row[14].replace(',', '.')
        gps_pair = (latitude, longitude)
        unique_gps[gps_pair] = unique_gps.get(gps_pair, 0) + 1


with open(output_file, "w") as f:
    f.write(
        '<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n'
        '<gpx xmlns="http://www.topografix.com/GPX/1/1" '
        'creator="Python GPX Library - https://github.com/tkrajina/gpxpy" '
        'version="1.1" '
        'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" '
        'xsi:schemaLocation="http://www.topografix.com/GPX/1/1 '
        'http://www.topografix.com/GPX/1/1/gpx.xsd">\n'
    )
    for gps_pair, count in unique_gps.items():
        print(f"{gps_pair[0]}, {gps_pair[1]}: {count} occurrences")
        description1 = f"{gps_pair[0]}, {gps_pair[1]}"
        description2 = f"{count} occurrences"
        f.write(
            f'<wpt lat="{gps_pair[0]}" lon="{gps_pair[1]}">\n'
            f'    <name>{description1}</name>\n'
            f'    <desc>{description2}</desc>\n'
            f'</wpt>\n'
        )
    f.write('</gpx>\n')
