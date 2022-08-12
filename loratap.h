/*
This file is part of LoRaTap Copyright (C) 2017 Erik de Jong

LoRaTap is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LoRaTap is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LoRaTap.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>

/*
	Values are big-endian
	
	Field details
	lt_version	LoRaTap header version, current version is 0. Set to 1 if loratap_extension_v1_t is present.
	lt_padding	Unused, for boundary alignment.
	lt_length	LoRaTap header length, field used to allow expansion in future versions. Each version can append header parameters. 
			So, whilst parsing, these parameters can be skipped if not defined for older versions by skipping to the offset of lt_length starting from the beginning of the header.
	channel		Channel information
	rssi		RSSI information
	sync_word	Sync word for received packet, value 0x34 is reserved for LoRaWAN packets
	loratap_extension_v1	Extension of LoRaTap protocol with additional SX1301 information.
*/

typedef enum sf { SF7=7, SF8, SF9, SF10, SF11, SF12 } sf_t;

typedef enum cr { CR_NONE=0, CR_4_5=5, CR_4_6=6, CR_4_7=7, CR_4_8=8 } cr_t;

typedef struct __attribute__((__packed__)) loratap_channel {
	uint32_t			frequency;	/* LoRa frequency (Hz) */
	uint8_t				bandwidth;	/* Channel bandwidth (KHz) in 125 KHz steps */
	uint8_t				sf;		/* LoRa SF (sf_t) [7, 8, 9, 10, 11, 12] */
} loratap_channel_t;

typedef struct __attribute__((__packed__)) loratap_rssi {
	uint8_t				packet_rssi;	/* LoRa packet RSSI, if snr >= 0 then dBm value is -139 + packet_rssi, otherwise dBm value is -139 + packet_rssi * .25; 255 for N/A */
	uint8_t				max_rssi;	/* LoRa receiver max RSSI (dBm value is -139 + rssi), 255 for N/A */
	uint8_t				current_rssi;	/* LoRa receiver current RSSI (dBm value is -139 + rssi), 255 for N/A */
	uint8_t				snr;		/* LoRa SNR (dB value is (snr[two's complement])/4) */
} loratap_rssi_t;

typedef struct __attribute__((__packed__)) loratap_header {
	uint8_t				lt_version;	/* LoRatap header version */
	uint8_t				lt_padding;
	uint16_t			lt_length;	/* LoRatap header length */
	loratap_channel_t		channel;
	loratap_rssi_t			rssi;
	uint8_t				sync_word;	/* LoRa radio sync word [0x34 = LoRaWAN] */
} loratap_header_t;

typedef struct __attribute__((__packed__)) loratap_extension_v1 {
	uint64_t			source_gw;	/* Source gateway ID */
	uint32_t			timestamp;	/* SX1301 tmst */
	uint8_t				mod_fsk:1;	/* FSK (1) or LoRa (0) modulation */
	uint8_t				implicit_hdr:1;	/* LoRa implicit header mode (Class-B beacon) */
	uint8_t				crc_ok:1;	/* Packet CRC valid */
	uint8_t				crc_bad:1;	/* Packet CRC invalid */
	uint8_t				no_crc:1;	/* Packet without CRC */
	uint8_t				padding:3;	/* Padding */
	uint8_t				cr;		/* LoRa coding rate (cr_t) [0, 5, 6, 7, 8] */
	uint8_t				channel;	/* SX1301 chan */
	uint8_t				radio;		/* SX1301 rfch */
} loratap_extension_v1_t;
