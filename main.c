#define CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "modules/peer.h"
#include "modules/webio.h"
#include "modules/config.h"
#include "lib/tcp-listener.h"

#pragma comment(lib, "ws2_32.lib")

int main(void) {
    Map config = config_load();

    FILE *seed_file;
    seed_file = fopen("seed.txt", "r");
    char seed[17];
    if (seed_file == NULL) {
        logger_log("Seed not found! Generating a new one...");
        strcpy(seed, generateSeed(16));
        seed_file = fopen("seed.txt", "w");
        fprintf(seed_file, "%s", seed);

    } else {
        fgets(seed, 512, seed_file);
    }
    fclose(seed_file);
    char id[18];
    strcpy(id, seed);


    char buf[513];


    Node_data mynode;
    strcpy(mynode.id, id);

    char *nickname = map_getValue(config, "nickname");
    if (nickname != NULL) {
        strcpy(mynode.nick, nickname);
    } else strcpy(mynode.nick, "");

    char *port = map_getValue(config, "port");
    if (port != NULL)
        mynode.port = atoi(port);
    else
        mynode.port = atoi(DEFAULT_PORT);

    logger_log("Initialising core...");
    //TODO: Ezt a részt külön függvénybe tenni egy külön file-ban


    struct addrinfo *result = NULL;
    SOCKET listening;
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

    FILE *peer_file;
    peer_file = fopen("peers.txt", "r");
    if (peer_file == NULL) {
        logger_log("peers.txt not found!");
        peer_file = fopen("peers.txt", "w");
        fprintf(peer_file, "\n");

    } else {
        char ip[NI_MAXHOST];
        int port;
        while (fscanf(peer_file, "%[^:]:%d", ip, &port) != EOF) {
            if (peer_ConnetctTo(ip, port, &peerList1, mynode, &master) != 0)
                logger_log("Error while connecting to peer...");
        }

    }
    fclose(peer_file);
    WebIO webIo;
    port = map_getValue(config, "interface-port");
    if (port == NULL)
        port = DEFAULT_INTERFACE_PORT;
    char *folder = map_getValue(config, "interface-folder");
    if (folder == NULL)
        folder = DEFAULT_WWW_FOLDER;
    char *local_mode_str = map_getValue(config, "interface-local");
    bool local_mode = false;
    if (strcmp(local_mode_str, "true") == 0)
        local_mode = true;
    res = webio_create(atoi(port), folder, mynode, !local_mode, &webIo);
    if (res != 0) {
        return EXIT_FAILURE;
    }
    FD_SET(webIo.socket, &master);

    logger_log("Started web interface at http://127.0.0.1:%d", tcp_getSockPort(webIo.socket));

    logger_log("Starting main loop...");
    bool run = true;
    while (run) {
        fd_set copy = master;
        SOCKET last = (peerList1.length > 0) ? peerList1.array[peerList1.length-1].socket : webIo.socket;
        int count = select(last+1, &copy, NULL, NULL, NULL);

        if (FD_ISSET(listening, &copy)) {
            if (peer_HandleConnection(listening, &peerList1, mynode, &master) != 0)
                logger_log("Error while receiving connection...");
        } else if (FD_ISSET(webIo.socket, &copy)) {
            res = webio_handleRequest(webIo, &peerList1);
            if (res == -2) {
                run = false;
            }
        } else {
            for (int i = 0; i < peerList1.length; i++) {
                SOCKET sock = peerList1.array[i].socket;
                if(!FD_ISSET(sock,&copy))
                    continue;
                char buf[DEFAULT_BUFLEN];
                memset(buf, 0, DEFAULT_BUFLEN);
                int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
                if (inBytes <= 0) {
                    //Peer disconnect
                    int k = peer_getPeer(peerList1, sock);
                    if (k != -1) {
                        logger_log("Peer disconnected(%s->%s)", inet_ntoa(peerList1.array[k].sockaddr.sin_addr),
                                   peerList1.array[k].peerData.id);
                        peer_removeFromList(&peerList1, k);
                        FD_CLR(sock, &master);
                    }
                } else {


                    if (strlen(buf) == 0)
                        continue;
                    Map m = getPacketData(buf);

                    char file[64];
                    int k = peer_getPeer(peerList1, sock);
                    sprintf(file, "%speers/%s.txt", DEFAULT_WWW_FOLDER, peerList1.array[k].peerData.id);
                    logger_log("Message received from %s", peerList1.array[k].peerData.id);
                    FILE *fp;
                    fp = fopen(file, "a");
                    fprintf(fp, "%s\n", map_getValue(m, "message"));
                    fclose(fp);
                    free(m.pairs);
                }
            }
        }
    }
    free(peerList1.array);
    free(config.pairs);
    logger_log("Closing socket...");
    close(listening);
    return 0;
}