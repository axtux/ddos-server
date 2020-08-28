#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <stdio.h>
#include <netdb.h>

#define MIN_PORT 1
#define MAX_PORT 65535
#define MAX_CONN 1024

void print_addr(unsigned long addr);
int tcp_serve(int port);

#endif // NETWORK_H_INCLUDED
