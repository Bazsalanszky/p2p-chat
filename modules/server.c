//
// Készítette: Toldi Balázs Ádám
// Dátum: 2019. 11. 07.
//

#include "server.h"

void getSeed(char *output) {
    FILE *seed_file;
    seed_file = fopen("seed.txt", "r");
    if (seed_file == NULL) {
        logger_log("Seed not found! Generating a new one...");
        char *string = generateSeed(16);
        strcpy(output, string);
        free(string);
        seed_file = fopen("seed.txt", "w");
        fprintf(seed_file, "%s", output);

    } else {
        fgets(output, 512, seed_file);
    }
    fclose(seed_file);
}

Node_data construct_Mynodedata(Config cfg) {
    Node_data result;
    getSeed(result.id);
    char *nickname = map_getValue(cfg, "nickname");
    if (nickname != NULL) {
        strcpy(result.nick, nickname);
    } else strcpy(result.nick, "");

    char *port = map_getValue(cfg, "port");
    if (port != NULL)
        result.port = atoi(port);
    else
        result.port = atoi(DEFAULT_PORT);

    return result;
}

void serverThread(SOCKET listening, fd_set *master, WebIO webIo, PeerList *list, Node_data mynode) {
    bool run = true;
    while (run) {
        fd_set copy = *master;
        SOCKET last = (list->length > 0) ? list->array[list->length - 1].socket : webIo.socket;
        int count = select(((int) last) + 1, &copy, NULL, NULL, NULL);

        if (FD_ISSET(listening, &copy)) {
            if (peer_HandleConnection(listening, list, mynode, master) != 0)
                logger_log("Error while receiving connection...");
        } else if (FD_ISSET(webIo.socket, &copy)) {
            int res = webio_handleRequest(webIo);
            if (res == -2) {
                run = false;
            }
        } else {
            for (int i = 0; i < (int) list->length; i++) {
                SOCKET sock = list->array[i].socket;
                if (!FD_ISSET(sock, &copy))
                    continue;
                char buf[DEFAULT_BUFLEN];
                memset(buf, 0, DEFAULT_BUFLEN);
                int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
                if (inBytes <= 0) {
                    //Peer disconnect
                    int k = peer_getPeer(*list, sock);
                    if (k != -1) {
                        logger_log("Peer disconnected(%s->%s)", inet_ntoa(list->array[k].sockaddr.sin_addr),
                                   list->array[k].peerData.id);
                        peer_removeFromList(list, k);
                        FD_CLR(sock, master);
                    }
                } else {


                    if (strlen(buf) == 0)
                        continue;
                    Map m = getPacketData(buf);
                    if (map_getValue(m, "message") != NULL) {
                        printf("%s\n", buf);
                        char file[64];
                        int k = peer_getPeer(*list, sock);
                        sprintf(file, "%speers/", webIo.folder);
#if defined(_WIN32)
                        CreateDirectoryA(file, NULL);
#else
                        mkdir(file, 0777);
#endif
                        sprintf(file, "%s%s.txt", file, list->array[k].peerData.id);
                        logger_log("Message received from %s", list->array[k].peerData.id);
                        FILE *fp;
                        fp = fopen(file, "a");

                        fprintf(fp, "%s\n", map_getValue(m, "message"));
                        fclose(fp);
                        free(m.pairs);
                    }
                }
            }
        }
    }
}
