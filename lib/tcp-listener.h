//
// Keszitette: Toldi Balázs Ádám
// Datum: 2019. 10. 28.
//
#pragma once
#include <ws2tcpip.h>
#include <stdbool.h>
#include "../utility.h"


int init_winsock(WSADATA* ws);

struct addrinfo* tcp_createIPv4Socket(SOCKET *s,int port,bool wildcard);
int tcp_bindnlisten(SOCKET s,struct addrinfo* addr,int conn_count);
struct sockaddr_in tcp_getAddr_in(SOCKET s);
int tcp_getSockPort(SOCKET s);