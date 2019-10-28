#include <stdio.h>
#include <stdlib.h>


#include "modules/peer.h"
#include "modules/crypto.h"
#include "modules/webio.h"
#include "modules/config.h"
#include "modules/tcp-listener.h"

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
        if(r == NULL){
            printOpenSSLError("Error generating RSA key pair!");
            return 2;
        }
        r = createRSAfromFile("private.pem",0);
    }

    char pub[16964];
    char priv[2049];
    RSA_getPublicKey(r,pub);
    RSA_getPrivateKey(r,priv);
    RSA_free(r);
    char buf[513];
    char id[MD5_DIGEST_LENGTH];

    md5(priv,id);
    node_data mynode;
    strcpy(mynode.id, id);
    strcpy(mynode.pubkey_str, pub);
    strcpy(mynode.privkey_str, priv);
    char *base64Key;
    base64Encode((unsigned char*)pub,strlen(pub),&base64Key);

    mynode.pubkey = createRSA((unsigned char*)mynode.pubkey_str,1);

    char * nickname = map_getValue(config,"nickname");
    if(nickname != NULL) {
        strcpy(mynode.nick, nickname);
    }else strcpy(mynode.nick, "");

    char * port = map_getValue(config,"port");
    if(port != NULL)
        mynode.port = atoi(port);
    else
    mynode.port = atoi(DEFAULT_PORT);

    logger_log("Initialising core...");
    //TODO: Ezt a részt külön függvénybe tenni egy külön file-ban
    WSADATA ws;
    int res = WSAStartup(MAKEWORD(2, 2), &ws);
    if (res != 0) {
        logger_log("Error at startup! Error code: %d", WSAGetLastError());
        WSACleanup();
    }

    struct addrinfo *result = NULL;
    SOCKET listening;
    result = tcp_createIPv4Socket(&listening,mynode.port,true);
    if(result == NULL){
        return EXIT_FAILURE;
    }
    res = tcp_bindnlisten(listening,result,SOMAXCONN);
    if(res != 0){
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
    port = map_getValue(config,"interface-port");
    if(port == NULL)
        port = DEFAULT_INTERFACE_PORT;
    char* folder = map_getValue(config,"interface-folder");
    if(folder == NULL)
        folder = DEFAULT_WWW_FOLDER;
    res = webio_create(atoi(port),folder,mynode,false,&webIo);
    if(res != 0){
        return EXIT_FAILURE;
    }
    FD_SET(webIo.socket,&master);

    logger_log("Started web interface at http://127.0.0.1:%d",tcp_getSockPort(webIo.socket));


    char *command =(char*) malloc(64);
    sprintf(command,"start http://127.0.0.1:%d",tcp_getSockPort(webIo.socket));
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