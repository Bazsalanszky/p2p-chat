//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14..
//
#pragma  once
#include "../../utility.h"
#include "../../peer.h"


typedef struct webio{
    SOCKET socket;
    struct sockaddr_in sockaddr;
    char folder[65];
    struct Node_data nodeData;
} WebIO;

int webio_create(int port,char* folder,struct Node_data myData,WebIO *webIo);

int webio_handleRequest(WebIO wio,peerList list);

int webio_handleGETrequest(SOCKET client,WebIO wio,char * file,peerList list);

int webio_handlePOSTrequest(SOCKET client,WebIO wio,peerList list,map post);

char* webio_getMIMEtype(char* filename);

char* webio_getFiletype(char* filename);

char* webio_getHeader(char* folder);

char* getIndex(char* folder,peerList list);
char* getPeerPage(char* folder,Peer p);