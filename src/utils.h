#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

void print_bytes(char *b, size_t len);
unsigned short check_sum(unsigned short *dgm, int bytes);
void create_datagram(char *datagram, const char *ip, const char *tcp);
void create_ip_header(char *ip, uint32_t saddr, uint32_t daddr);
void create_tcp_header(char *tcp, uint16_t source, uint16_t dest);

#endif
