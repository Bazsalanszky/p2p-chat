#define CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "modules/peer.h"
#include "modules/webio.h"
#include "modules/config.h"
#include "modules/server.h"
#include "lib/tcp-listener.h"
#include "lib/debugmalloc/debugmalloc.h"

SOCKET listening;
SOCKET web_sock;

void closeSocks(){
    logger_log("Closing socket...");
    closesocket(listening);
    closesocket(web_sock);
}

int main(void) {
    atexit(closeSocks);
    Map config = config_load();

    Node_data mynode = construct_Mynodedata(config);
    logger_log("Initialising core...");

    #if defined(WIN32)
    WSADATA ws;
	
    int r1 = WSAStartup(MAKEWORD(2,2),&ws);
    if(r1 != 0){
        logger_log("Error at WSAStartup.");
        WSACleanup();
        return EXIT_FAILURE;
    }
    #endif

    struct addrinfo *result = NULL;
    result = tcp_createIPv4Socket(&listening, mynode.port, true);
    if (result == NULL) {
        return EXIT_FAILURE;
    }
    int res = tcp_bindnlisten(listening, result, SOMAXCONN);
    if (res != 0) {
        return EXIT_FAILURE;
    }
    //Ez alapvetően akkor hasznos amikor a port 0-ra van állítva, azaz akkor amikor a rendszer random választ egyet.

    mynode.port = tcp_getSockPort(listening);
    logger_log("Started listening on port %d", mynode.port);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(listening, &master);

    //Connecting to peers
    logger_log("Checking peers.txt for peers...");
    PeerList peerList1;
    peer_initList(&peerList1);

    WebIO webIo;

    res = webio_create(config,mynode, &webIo);
    if (res != 0) {
        return EXIT_FAILURE;
    }
    FD_SET(webIo.socket, &master);
    web_sock = webIo.socket;
    logger_log("Started web interface at http://127.0.0.1:%d", tcp_getSockPort(webIo.socket));
    peer_loadPeerList(&peerList1,mynode,&master);
    logger_log("Starting main loop...");

    serverThread(listening,&master,webIo,peerList1,mynode);
    if(peerList1.size >0)free(peerList1.array);
    //Ezzel mi a baj?
    if(config.size > 0) free(config.pairs);
    closeSocks();
    return 0;
}