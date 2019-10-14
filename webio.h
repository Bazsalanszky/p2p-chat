//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14..
//
#ifndef P2P_UTILITY_H
#include "utility.h"
#endif
typedef struct webio{
    SOCKET socket;
    struct sockaddr_in sockaddr;
    char folder[65];
} WebIO;

int webio_create(int port,char* folder,WebIO *webIo);

int webio_handleRequest(WebIO wio);
