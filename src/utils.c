#include "utils.h"
#include <stdio.h>
#include <string.h>

char ip_frame[20] = {
    0x45, 0x00, 0x28, 0x00,
    0x1c, 0x46, 0x40, 0x00,
    0x40, 0x06, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

char tcp_frame[20] = {
    0x30, 0x39, 0x26, 0xc9,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x50, 0x02, 0x16, 0xd0,
    0x00, 0x00, 0x00, 0x00,
};

void print_bytes(char *b, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X", b[i]);

        if ((i + 1) % 2 == 0) {
            printf(" ");
        }

        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
}

unsigned short check_sum(unsigned short *dgm, int bytes) {
    register long sum = 0;
    register short answer;
    unsigned int odd_byte;

    while(bytes > 1) {
        sum += *dgm++;
        bytes -= 2;
    }

    if(bytes == 1) {
        odd_byte = 0;
        *((unsigned char*)&odd_byte) = *(unsigned char*)dgm;
        sum += odd_byte;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    answer = (short)~sum;

    return answer;
}

void create_datagram(char *datagram, const char *ip, const char *tcp) {
    memset(datagram, 0, 40);

    memcpy(datagram, ip, 20);
    unsigned short ip_checksum = check_sum((unsigned short *)datagram, 40);
    datagram[10] = ip_checksum >> 8;
    datagram[11] = ip_checksum;

    // set pseudo header for tcp checksum calculation
    memcpy(datagram+20, tcp, 20);

    char pseudo_header[32] = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x06, 0x00, 0x14,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    memcpy(pseudo_header,    ip+12,  4);
    memcpy(pseudo_header+4,  ip+16,  4);
    memcpy(pseudo_header+12, tcp,   20);

    unsigned short tcp_checksum = check_sum((unsigned short *)pseudo_header, 32);
    datagram[26] = tcp_checksum;
    datagram[27] = tcp_checksum >> 8;
}

void create_ip_header(char *ip, uint32_t saddr, uint32_t daddr) {
    memcpy(ip, ip_frame, 20);
    memcpy(ip+12, (char *)&saddr, 4);
    memcpy(ip+16, (char *)&daddr, 4);
}

void create_tcp_header(char *tcp, uint16_t source, uint16_t dest) {
    memcpy(tcp, tcp_frame, 20);
    memcpy(tcp, (char *)&source, 2);
    memcpy(tcp+2, (char *)&dest, 2);
}
