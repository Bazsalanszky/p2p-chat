#include <stdio.h>
#include <stdlib.h>

#include "peer.h"
#include "modules/crypto.h"
#include "modules/webio/webio.h"
#include "modules/config.h"

#pragma comment(lib, "ws2_32.lib")

#ifdef RANDOM_PORT
#define DEFAULT_PORT "0"
#else
#define DEFAULT_PORT "6327"
#endif
#define DEFAULT_INTERFACE_PORT "5081"
#define DEFAULT_WWW_FOLDER "htdocs/"


int main(void) {
    map config = config_load();

    RSA* r = createRSAfromFile("private.pem",0);
    if(r == NULL){
        logger_log("RSA key not found! Generating a new one...");
        r = generate_key();

    }

    FILE *pubkey;
    pubkey = fopen("public.pem", "r");
    char pub[513];
    char buf[513];
    char id[MD5_DIGEST_LENGTH];
    ZeroMemory(pub,513);
    while(fgets(buf,513,pubkey)!= NULL){
        strcat(pub,buf);
    }
    md5(pub,id);
    node_data mynode;
    strcpy(mynode.id, id);
    strcpy(mynode.pubkey_str, pub);
    char *base64Key;
    base64Encode((unsigned char*)pub,strlen(pub),&base64Key);

    mynode.pubkey = createRSA((unsigned char*)mynode.pubkey_str,1);
    if(map_isFound(config,"nickname")) {
        strcpy(mynode.nick, map_getValue(config, "nickname"));
    }
    if(map_isFound(config,"port"))
        mynode.port = atoi(map_getValue(config,"port"));
    else
    mynode.port = atoi(DEFAULT_PORT);
    logger_log("Initialising core...");
    WSADATA ws;
    int res = WSAStartup(MAKEWORD(2, 2), &ws);
    if (res != 0) {
        logger_log("Error at startup! Error code: %d", WSAGetLastError());
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
    char sport[10];
    sprintf( sport, "%d", mynode.port);
    res = getaddrinfo(NULL, sport, &hint, &result);
    if (res != 0) {
        logger_log("Error creating address information! Error code: %d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    //Creating listening socket
    SOCKET listening = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listening == INVALID_SOCKET) {
        logger_log("Error creating socket! Error: %d", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    //Binding the socket
    res = bind(listening, result->ai_addr, result->ai_addrlen);
    if (res == SOCKET_ERROR) {
        logger_log("Error binding socket! Error: %d", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return EXIT_FAILURE;
    }

    freeaddrinfo(result);

    //TODO: Set max connection count in config
    res = listen(listening, SOMAXCONN);
    if (res == -1) {
        logger_log("Error starting listening! Error: %d", WSAGetLastError());
        closesocket(listening);
        WSACleanup();
        return EXIT_FAILURE;
    }
    //Ez alapvetően akkor hasznos amikor a port 0-ra van állítva, azaz akkor amikor a rendszer random választ egyet.
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(listening, (struct sockaddr *) &sin, &len) == -1) {
        logger_log("Error at getsockname!Error code: %d", WSAGetLastError());
        closesocket(listening);
        WSACleanup();
        return 1;
    }
    mynode.port = ntohs(sin.sin_port);
    logger_log("Started listening on port %d", mynode.port);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(listening, &master);

    //Connecting to peers
    logger_log("Checking peers.txt for peers...");
    peerList peerList1;
    peer_initList(&peerList1);

    FILE *peer_file;
    peer_file = fopen("peers.txt", "r");
    if (peer_file == NULL) {
        logger_log("peers.txt not found!");
        peer_file = fopen("peers.txt", "w");
        fprintf(peer_file, "");

    } else {
        char ip[NI_MAXHOST];
        int port;
        while (fscanf(peer_file, "%[^:]:%d", ip, &port) != EOF) {
            if(peer_ConnetctTo(ip, port, &peerList1, mynode,&master) != 0)
                logger_log("Error while connecting to peer...");
        }

    }
    fclose(peer_file);
    WebIO webIo;
    webio_create(atoi(DEFAULT_INTERFACE_PORT),DEFAULT_WWW_FOLDER,mynode,&webIo);
    FD_SET(webIo.socket,&master);

    logger_log("Started web interface at http://127.0.0.1:%d",ntohs(webIo.sockaddr.sin_port));


    char *command =(char*) malloc(64);
    sprintf(command,"start http://127.0.0.1:%d",ntohs(webIo.sockaddr.sin_port));
    system(command);
    free(command);

    logger_log("Starting main loop...");
    bool run = true;
    while (run) {
        fd_set copy = master;
        int count = select(0, &copy, NULL, NULL, NULL);

        for (int i = 0; i < count; i++) {
            SOCKET sock = copy.fd_array[i];
            if (sock == listening) {
                if(peer_HandleConnection(listening, &peerList1, mynode,&master) != 0)
                    logger_log("Error while receiving connection...");
            }else if(sock == webIo.socket ){
                res = webio_handleRequest(webIo,peerList1);
                if(res == -2){
                    run = false;
                }
            } else {
                char buf[DEFAULT_BUFLEN];
                ZeroMemory(buf, DEFAULT_BUFLEN);
                int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
                if (inBytes <= 0) {
                    //Peer disconnect
                    int k = peer_getPeer(peerList1, sock);
                    if (k != -1) {
                        logger_log("Peer disconnected(%s->%s)", inet_ntoa(peerList1.array[k].sockaddr.sin_addr),peerList1.array[k].peerData.id);
                        peer_removeFromList(&peerList1, k);
                        closesocket(sock);
                        FD_CLR(sock, &master);
                    }
                }else{
                    if(strlen(buf) ==0)
                        continue;
                    map m = getHandshakeData(buf);
                    map_dump(m);
                    char file[64];
                    int k = peer_getPeer(peerList1, sock);
                    sprintf(file,"%s%s.txt",DEFAULT_WWW_FOLDER,peerList1.array[k].peerData.id);
                    logger_log("Message received from %s",peerList1.array[k].peerData.id);
                    FILE *fp;
                    fp = fopen(file,"a");
                    fprintf(fp,"%s\n",map_getValue(m,"message"));
                    fclose(fp);
                }
            }
        }
    }
    free(peerList1.array);
    free(config.pairs);
    logger_log("Closing socket...");
    closesocket(listening);
    WSACleanup();
    return 0;
}