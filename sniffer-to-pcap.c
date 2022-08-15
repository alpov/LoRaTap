#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#define __USE_XOPEN
#include <time.h>
#include <byteswap.h>
#include <arpa/inet.h>
#include <cjson/cJSON.h>
#include "loratap.h"
#include "base64.h"

#define LINKTYPE_LORA_LORATAP 270

typedef struct pcap_hdr_s {
    uint32_t magic_number;   /* magic number */
    uint16_t version_major;  /* major version number */
    uint16_t version_minor;  /* minor version number */
    int32_t  thiszone;       /* GMT to local correction */
    uint32_t sigfigs;        /* accuracy of timestamps */
    uint32_t snaplen;        /* max length of captured packets, in octets */
    uint32_t network;        /* data link type */
} pcap_hdr_t;

typedef struct pcaprec_hdr_s {
    uint32_t ts_sec;         /* timestamp seconds */
    uint32_t ts_usec;        /* timestamp microseconds */
    uint32_t incl_len;       /* number of octets of packet saved in file */
    uint32_t orig_len;       /* actual length of packet */
} pcaprec_hdr_t;

// https://opensource.apple.com/source/QuickTimeStreamingServer/QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.c
/* aaaack but it's fast and const should make it shared text page. */
static const unsigned char pr2six[256] =
{
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

int Base64decode(char *bufplain, const char *bufcoded)
{
    int nbytesdecoded;
    register const unsigned char *bufin;
    register unsigned char *bufout;
    register int nprbytes;

    bufin = (const unsigned char *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    bufout = (unsigned char *) bufplain;
    bufin = (const unsigned char *) bufcoded;

    while (nprbytes > 4) {
    *(bufout++) =
        (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    *(bufout++) =
        (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    *(bufout++) =
        (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    bufin += 4;
    nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
    *(bufout++) =
        (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
    *(bufout++) =
        (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
    *(bufout++) =
        (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }

    *(bufout++) = '\0';
    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
}

int main(int argc, char *argv[])
{
    FILE *captureFile;
    pcap_hdr_t file_header;
    FILE *inputFile;
    char *line = NULL;
    size_t len = 0;
    bool enable_extension_v1 = false;
    
    if (argc != 4) {
        printf("Usage: %s {v0|v1} input_file output_file\n", argv[0]);
        return 0;
    }
    
    if (strcmp(argv[1], "v1") == 0) {
        enable_extension_v1 = true;
    }
    
    inputFile = fopen(argv[2], "r");
    
    /* Create pcap with header */
    captureFile = fopen(argv[3], "w");
    file_header.magic_number = 0xa1b2c3d4;
    file_header.version_major = 2;
    file_header.version_minor = 4;
    file_header.thiszone = 0;
    file_header.sigfigs = 0;
    file_header.snaplen = 255;
    file_header.network = LINKTYPE_LORA_LORATAP;
    fwrite(&file_header, sizeof(pcap_hdr_t), 1, captureFile);
    
    while (getline(&line, &len, inputFile) != -1) {
        if (strlen(line) < 20) continue;
        char *addr_txt = line; addr_txt[18] = '\0';
        char *json_txt = line + 20;
        //printf("addr: '%s', json: '%s'\n", addr_txt, json_txt);

        cJSON *json = cJSON_Parse(json_txt);
        const cJSON *rxpk = NULL;
        cJSON_ArrayForEach(rxpk, cJSON_GetObjectItemCaseSensitive(json, "rxpk")) {
            if (!cJSON_HasObjectItem(rxpk, "time")) {
                printf("-");
                continue;
            }
            
            int		tmst = cJSON_GetObjectItemCaseSensitive(rxpk, "tmst")->valueint;
            char *	time = cJSON_GetObjectItemCaseSensitive(rxpk, "time")->valuestring;
            //int	tmms = cJSON_GetObjectItemCaseSensitive(rxpk, "tmms")->valueint;
            int		chan = cJSON_GetObjectItemCaseSensitive(rxpk, "chan")->valueint;
            int		rfch = cJSON_GetObjectItemCaseSensitive(rxpk, "rfch")->valueint;
            double	freq = cJSON_GetObjectItemCaseSensitive(rxpk, "freq")->valuedouble;
            int		stat = cJSON_GetObjectItemCaseSensitive(rxpk, "stat")->valueint;
            char *	modu = cJSON_GetObjectItemCaseSensitive(rxpk, "modu")->valuestring;
            char *	datr = cJSON_GetObjectItemCaseSensitive(rxpk, "datr")->valuestring;
            char *	codr = cJSON_GetObjectItemCaseSensitive(rxpk, "codr")->valuestring;
            double	lsnr = cJSON_GetObjectItemCaseSensitive(rxpk, "lsnr")->valuedouble;
            double	rssi = cJSON_GetObjectItemCaseSensitive(rxpk, "rssi")->valuedouble;
            int		size = cJSON_GetObjectItemCaseSensitive(rxpk, "size")->valueint;
            char *	data = cJSON_GetObjectItemCaseSensitive(rxpk, "data")->valuestring;
            
            int sf = 0, bw = 0;
            sscanf(datr, "SF%dBW%d", &sf, &bw);
            
            int cr = 0;
            if (strcmp(codr, "OFF") != 0) {
                sscanf(codr, "4/%d", &cr);
            }
            
            if (chan == 8 /*stat == 0 && freq == 869.525 && sf == 9 && bw == 125*/) {
                printf("b"); // Class-B beacon
            } else if (stat != 1) {
                printf("X");
                if (!enable_extension_v1) {
                    // do not pass packets with wrong CRC if not marked in extension header
                    continue; // skip wrong or missing CRC
                }
            } else {
                printf("."); // CRC OK
            }
            
            struct tm tm;
            strptime(time, "%Y-%m-%dT%H:%M:%S", &tm);
            
            /* Write header */
            pcaprec_hdr_t pcap_packet_header = {0};
            pcap_packet_header.ts_sec = timegm(&tm);
            pcap_packet_header.ts_usec = atoi(&time[20]);
            if (enable_extension_v1) {
                pcap_packet_header.incl_len = size + sizeof(loratap_header_t) + sizeof(loratap_extension_v1_t);
                pcap_packet_header.orig_len = size + sizeof(loratap_header_t) + sizeof(loratap_extension_v1_t);
            } else {
                pcap_packet_header.incl_len = size + sizeof(loratap_header_t);
                pcap_packet_header.orig_len = size + sizeof(loratap_header_t);
            }
            fwrite(&pcap_packet_header, sizeof(pcaprec_hdr_t), 1, captureFile);
            
            /* Write packet */
            loratap_header_t loratap_packet_header = {0};
            if (enable_extension_v1) {
                loratap_packet_header.lt_version = 1;
                loratap_packet_header.lt_length = htons(sizeof(loratap_header_t) + sizeof(loratap_extension_v1_t));
            } else {
                loratap_packet_header.lt_version = 0;
                loratap_packet_header.lt_length = htons(sizeof(loratap_header_t));
            }
            loratap_packet_header.channel.frequency = htonl((uint32_t)(freq * 1000000.));
            loratap_packet_header.channel.bandwidth = bw / 125;
            loratap_packet_header.channel.sf = sf;
            loratap_packet_header.rssi.packet_rssi = 255;
            loratap_packet_header.rssi.current_rssi = (uint8_t)(rssi + 139.);
            loratap_packet_header.rssi.max_rssi = 255;
            loratap_packet_header.rssi.snr = (uint8_t)(lsnr * 4.);
            loratap_packet_header.sync_word = 0x34; // always LoRaWAN, was: (chan == 8) ? 0xAA : 0x34;
            fwrite(&loratap_packet_header, sizeof(loratap_header_t), 1, captureFile);
            
            if (enable_extension_v1) {
                /* Extension header v1 */
                loratap_extension_v1_t loratap_extension_v1 = {0};
                loratap_extension_v1.source_gw = bswap_64(strtoull(addr_txt, NULL, 0));
                loratap_extension_v1.timestamp = htonl((uint32_t)tmst);
                loratap_extension_v1.mod_fsk = (strcmp(modu, "FSK") == 0) ? 1 : 0;
                loratap_extension_v1.implicit_hdr = (chan == 8 && stat == 0) ? 1 : 0; // Implicit header on channel 8 with CRC check disabled
                loratap_extension_v1.crc_ok = (stat == 1) ? 1 : 0;
                loratap_extension_v1.crc_bad = (stat == -1) ? 1 : 0;
                loratap_extension_v1.no_crc = (stat == 0) ? 1 : 0;
                loratap_extension_v1.cr = cr;
                loratap_extension_v1.channel = chan;
                loratap_extension_v1.radio = rfch;
                fwrite(&loratap_extension_v1, sizeof(loratap_extension_v1_t), 1, captureFile);
            }
            
            /* Write payload */
            char data_raw[2048];
            int data_len = Base64decode(data_raw, data);
            assert(size == data_len);
            fwrite(data_raw, size, 1, captureFile);
        }
        cJSON_Delete(json);
    }
    
    fclose(inputFile);
    fclose(captureFile);
    free(line);
    printf("\ndone\n");
    
    return 0;
}
