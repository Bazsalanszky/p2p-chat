//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14.
//

#pragma  once
#define  _CRT_SECURE_NO_WARNINGS
#include "../utility.h"
#include "../lib/tcp-listener.h"
#include "peer.h"
#include "config.h"
#include "../lib/debugmalloc/debugmalloc.h"




typedef struct webio{
    SOCKET socket;
    struct sockaddr_in sockaddr;
    char folder[65];
    struct Node_data nodeData;
} WebIO;

int webio_create(Config config, struct Node_data myData, WebIO *webIo);

int webio_handleRequest(WebIO wio, const PeerList *list);

static int webio_handleGETrequest(SOCKET client, WebIO wio, char * file, const PeerList *list);

static int webio_handlePOSTrequest(SOCKET client, WebIO wio, const PeerList *list, Map post);

static char* webio_getMIMEtype(char* filename);

static char* webio_getFiletype(char* filename);

static void webio_getHeader(char* folder, char result[]);

static void webio_getIndex(char* folder, const PeerList *list, char *outputBuffer);

static void webio_getPeerPage(char* folder, char *id, bool online, char *outputBuffer);

static bool webio_isPeerFound(char* folder,char* id);

static void webio_sendOKHeader(SOCKET socket,char* file);

static void webio_sendOKHeader_wSize(SOCKET socket,char* file,int size);

static void webio_sendPage(SOCKET socket, char* content);

static void webio_send404Page(SOCKET socket);