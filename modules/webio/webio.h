//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14..
//
#ifndef P2P_UTILITY_H
#include "../../utility.h"
#endif
#ifndef  P2P_PEER_H
#include "../../peer.h"
#endif

#ifndef P2P_WEBIO_H
#define P2P_WEBIO_H
#endif

typedef struct webio{
    SOCKET socket;
    struct sockaddr_in sockaddr;
    char folder[65];
    struct Node_data nodeData;
} WebIO;

int webio_create(int port,char* folder,struct Node_data myData,WebIO *webIo);

int webio_handleRequest(WebIO wio,Peer list[],int peerCount);

int webio_handleGETrequest(SOCKET client,WebIO wio,char * file,Peer list[],int peerCount);

int webio_handlePOSTrequest(SOCKET client,WebIO wio,Peer list[],int peerCount);

char* webio_getMIMEtype(char* filename);

char* webio_getFiletype(char* filename);

char* webio_getHeader(char* folder);

char* getIndex(char* folder,Peer list[],int count);