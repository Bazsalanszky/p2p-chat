#include <stdio.h>
#include <stdlib.h>
#include "peer.h"
#include "webio.h"

#pragma comment(lib, "ws2_32.lib")
//#include "webinterface.h"

#ifdef RANDOM_PORT
#define DEFAULT_PORT "0"
#else
#define DEFAULT_PORT "6327"
#endif
#define DEFAULT_INTERFACE_PORT "5081"
#define DEFAULT_WWW_FOLDER "htdocs"


int main(void) {
    FILE *seed_file;
    seed_file = fopen("seed.txt", "r");
    char seed[513];
    if (seed_file == NULL) {
        printf("Seed not found! Generating a new one...\n");
        strcpy(seed, generateSeed(512));
        seed_file = fopen("seed.txt", "w");
        fprintf(seed_file, "%s", seed);

    } else {
        printf("Seed found!\n");
        fgets(seed, 512, seed_file);
    }
    fclose(seed_file);
    char id[MD5_DIGEST_LENGTH];
    md5(seed, id);
    node_data mynode;
    strcpy(mynode.id, id);
    strcpy(mynode.nick, "");
    mynode.port = atoi(DEFAULT_PORT);

    printf("Initialising core...\n");
    WSADATA ws;
    int res = WSAStartup(MAKEWORD(2, 2), &ws);
    if (res != 0) {
        printf("Error at startup! Error code: %d", WSAGetLastError());
        WSACleanup();
    }

    struct addrinfo hint = {};
    struct addrinfo *result = NULL;

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
    if (listening == INVALID_SOCKET) {
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
    res = listen(listening, SOMAXCONN);
    if (res == -1) {
        printf("Error starting listening! Error: %d", WSAGetLastError());
        closesocket(listening);
        WSACleanup();
        return EXIT_FAILURE;
    }
    //Ez alapvetően akkor hasznos amikor a port 0-ra van állítva, azaz akkor amikor a rendszer random választ egyet.
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(listening, (struct sockaddr *) &sin, &len) == -1) {
        printf("Error at getsockname!Error code: %d", WSAGetLastError());
        return 1;
    }
    mynode.port = ntohs(sin.sin_port);
    printf("Started listening on port %d\n", mynode.port);


    fd_set master;
    FD_ZERO(&master);
    FD_SET(listening, &master);

    //Connecting to peers
    printf("Checking peers.txt for peers...\n");
    Peer *peerList = (Peer *) malloc(sizeof(Peer) * DEFAULT_MAX_PEER_COUNT);
    int peerCount = 0;

    FILE *peer_file;
    peer_file = fopen("peers.txt", "r");
    if (peer_file == NULL) {
        printf("peers.txt not found!\n");
        peer_file = fopen("peers.txt", "w");
        fprintf(peer_file, "");

    } else {
        char ip[NI_MAXHOST];
        int port;
        while (fscanf(peer_file, "%[^:]:%d", ip, &port) != EOF) {
            if(peer_ConnetctTo(ip, port, peerList, &peerCount, mynode,&master) != 0)
                printf("Error while connecting to peer...\n");
        }

    }
    fclose(peer_file);
    WebIO webIo;
    webio_create(atoi(DEFAULT_INTERFACE_PORT),DEFAULT_WWW_FOLDER,&webIo);
    FD_SET(webIo.socket,&master);
    printf("Started web interface at http://127.0.0.1:%d\n",ntohs(webIo.sockaddr.sin_port));
    char *command =(char*) malloc(64);
    sprintf(command,"start http://127.0.0.1:%d",ntohs(webIo.sockaddr.sin_port));
    //system(command);

    printf("Starting main loop...\n");
    while (1) {
        fd_set copy = master;
        int count = select(0, &copy, NULL, NULL, NULL);

        for (int i = 0; i < count; i++) {
            SOCKET sock = copy.fd_array[i];
            if (sock == listening) {
                if(peer_HandleConnection(listening, peerList, &peerCount, mynode,&master) != 0)
                    printf("Error while receiving connection...\n");
            }else if(sock == webIo.socket ){
                webio_handleRequest(webIo);
            } else {
                char buf[DEFAULT_BUFLEN];
                ZeroMemory(buf, DEFAULT_BUFLEN);
                int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
                if (inBytes <= 0) {
                    //Peer disconnect
                    int k = peer_getPeer(peerList, peerCount, sock);
                    if (k != -1) {
                        printf("Peer disconnected(%s->%s)\n", inet_ntoa(peerList[k].sockaddr.sin_addr),peerList[k].peerData.id);
                        peer_removeFromList(peerList, &peerCount, k);
                        closesocket(sock);
                        FD_CLR(sock, &master);
                    }
                }
            }
        }
    }


    free(peerList);
    closesocket(listening);
    WSACleanup();
    return 0;
}