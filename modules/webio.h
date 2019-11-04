//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14..
//
#pragma  once
#include "../utility.h"
#include "../lib/tcp-listener.h"
#include "peer.h"


typedef struct webio{
    SOCKET socket;
    struct sockaddr_in sockaddr;
    char folder[65];
    struct Node_data nodeData;
} WebIO;

int webio_create(int port,char* folder,struct Node_data myData,bool wildcard,WebIO *webIo);

int webio_handleRequest(WebIO wio,const PeerList *list);

int webio_handleGETrequest(SOCKET client,WebIO wio,char * file,const PeerList *list);

int webio_handlePOSTrequest(SOCKET client, WebIO wio, const PeerList *list, Map post);

char* webio_getMIMEtype(char* filename);

char* webio_getFiletype(char* filename);

void webio_getHeader(char* folder, char result[]);

void webio_getIndex(char* folder, const PeerList *list, char *outputBuffer);

void webio_getPeerPage(char* folder, char *id, bool online, char *outputBuffer);

bool webio_isPeerFound(char* folder,char* id);