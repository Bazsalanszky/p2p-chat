#include <stdio.h>
#include <stdlib.h>
#include "peer.h"

#pragma comment(lib,"ws2_32.lib")
//#include "webinterface.h"


#define DEFAULT_PORT "6328"
#define DEFAULT_INTERFACE_PORT "5081"
#define DEFAULT_WWW_FOLDER "htdocs"




int main(void) {
    printf("Initialising core...\n");
    WSADATA ws;
    int res = WSAStartup(MAKEWORD(2,2),&ws);
    if(res != 0){
        printf("Error at startup! Error code: %d",WSAGetLastError());
        WSACleanup();
    }
    struct  peer peerList[DEFAULT_MAX_PEER_COUNT];
    node_data md;
    //TODO: Load these from config
    strcpy(md.id, generateSeed(512));
    md.port = DEFAULT_PORT;

    struct addrinfo hint = {};
    struct addrinfo* result = NULL;

    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_INET;
    //TODO: Disable this in local mode
    hint.ai_flags = AI_PASSIVE;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    //TODO: Use config to determine port
    res = getaddrinfo(NULL, DEFAULT_PORT, &hint, &result);

    if (res != 0) {
        printf("Error creating address information! Error code: %d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    //Creating listening socket
    SOCKET listening = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(listening == INVALID_SOCKET){
        printf("Error creating socket! Error: %d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    //Binding the socket
    res = bind(listening, result->ai_addr, result->ai_addrlen);
    if (res == SOCKET_ERROR) {
        printf("Error binding socket! Error: %d", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return EXIT_FAILURE;
    }

    freeaddrinfo(result);

    //TODO: Set max connection count in config
    res = listen(listening,SOMAXCONN);
    if (res == -1) {
        printf("Error starting listening! Error: %d", WSAGetLastError());
        closesocket(listening);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Started listening on port %s",DEFAULT_PORT);
    //TODO: Create interface

    fd_set  master;
    FD_ZERO(&master);
    FD_SET(listening,&master);

    while(1){
        fd_set copy = master;
        int count = select(0, &copy, NULL, NULL, NULL);
        for (int i = 0; i < count;i++) {
            SOCKET sock = copy.fd_array[i];
            if(sock == listening){
                peer_HandleConnection(listening,peerList,DEFAULT_MAX_PEER_COUNT,md);
            }
        }

    }
    closesocket(listening);
    return 0;
}