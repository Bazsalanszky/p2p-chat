//
// Készítette: Toldi Balázs Ádám
// Dátum: 2019. 10. 11.
//
#include "peer.h"

//Kouhai peer
//Amikor mi csatlakozunk egy peerhez
int peer_ConnetctTo(char *ip, int port, PeerList *peerList, Node_data my, fd_set *fdSet) {
    if (strcmp(ip, "0.0.0.0") == 0)
        return 0;
    if (peer_IP_isFound(*peerList, ip, port))
        return 1;
    struct sockaddr_in hint;
    SOCKET sock = initPeer(ip, port, &hint);
    if (sock == INVALID_SOCKET) {
        logger_log("Error initialising peer socket!");
        return 1;
    }
    logger_log("Connected to peer!Sending handshake...");
    char handshake[DEFAULT_BUFLEN];
    constructHandshake(my,NULL,handshake);
    int res = send(sock, handshake, strlen(handshake), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending handshake!Disconnecting...");
        printLastError();
        closesocket(sock);
        return -1;
    }
    logger_log("Sent!Waiting for response...");
    char buf[DEFAULT_BUFLEN];
    memset(buf, 0, DEFAULT_BUFLEN);
    int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
    if (inBytes <= 0) {
        logger_log("Error: Invalid response!");
        printLastError();
        closesocket(sock);
        return -1;
    }
    if (handshake[0] != '@') {
        logger_log("Error: Invalid response!");
        sendErrorMSG("INVALID_RESPONSE", sock);
        closesocket(sock);
        return -1;
    }
    Node_data node;
    strcpy(node.ip, ip);
    Map m = getPacketData(buf);
    if (constructNodeData(&m, peerList, &sock, &node) != 0)
        return 1;

    Peer p;
    p.peerData = node;
    p.socket = sock;
    p.sockaddr = hint;
    FD_SET(sock, fdSet);
    peer_addTolist(peerList, p);
    char *peers = map_getValue(m, "peers");
    connectToReceivedPeers(peers, node, fdSet, peerList);

    free(m.pairs);
    logger_log("Peer validated (%s->%s)!", inet_ntoa(hint.sin_addr), node.id);
    return 0;
}

//Senpai peer
//Amikor a egy peer csatlakozik hozzánk
int peer_HandleConnection(SOCKET listening, PeerList *peerList, Node_data my, fd_set *fdSet) {
    struct sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET sock = accept(listening, (struct sockaddr *) &client, &clientSize);
    char ip[NI_MAXHOST];
    memset(ip, 0, NI_MAXHOST);
    inet_ntop(AF_INET, &client.sin_addr, ip, NI_MAXHOST);
    if (strcmp(ip, "0.0.0.0") == 0)
        return 0;

    logger_log("Incoming connection from %s...", ip);
    char buf[DEFAULT_BUFLEN];
    memset(buf, 0, DEFAULT_BUFLEN);
    int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
    if (inBytes <= 0) {
        logger_log("Error: Invalid response!");
        printLastError();
        closesocket(sock);
        return -1;
    }
    if (buf[0] != '@') {
        logger_log("Error: Invalid response!");
        sendErrorMSG("INVALID_RESPONSE", sock);
        closesocket(sock);
        return -1;
    }
    Map m = getPacketData(buf);
    Node_data node;
    strcpy(node.ip, ip);
    if (constructNodeData(&m, peerList, &sock, &node) != 0)
        return 1;
    free(m.pairs);
    logger_log("Handshake recived! Sending response!");
    char handshake[DEFAULT_BUFLEN];
    constructHandshake(my,peerList,handshake);
    int res = send(sock, handshake, strlen(handshake), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending handshake!Disconnecting...");
        closesocket(sock);
        return -1;
    }
    Peer p;
    p.peerData = node;
    p.socket = sock;
    p.sockaddr = client;
    peer_addTolist(peerList, p);
    FD_SET(sock, fdSet);

    logger_log("Peer successfully connected from %s with id %s", inet_ntoa(client.sin_addr), node.id);
    return 0;
}

void peer_initList(PeerList *list) {
    list->size = 0;
    list->length = 0;
    list->array = 0;
}


bool peer_ID_isFound(PeerList list, char *id) {
    for (size_t i = 0; i < list.length; ++i) {
        if (strcmp(list.array[i].peerData.id, id) == 0) {
            return true;
        }
    }
    return false;
}

bool peer_IP_isFound(struct PeerList list, char *ip, int port) {
    for (size_t i = 0; i < list.length; ++i) {
        if (strcmp(list.array[i].peerData.ip, ip) == 0 && list.array[i].peerData.port == port) {
            return true;
        }
    }
    return false;
}


void peer_addTolist(PeerList *list, struct peer p) {
    if (list->length >= list->size) {
        assert(list->length == list->size);
        size_t new_size = (list->size + 2) * 2;
        Peer *new_list = realloc(list->array, new_size * sizeof(Peer));
        if (new_list == 0)
            printf("OUT OF MEMORY!");
        list->size = new_size;
        list->array = new_list;
    }
    list->array[list->length++] = p;
}

void peer_removeFromList(struct PeerList *list, int i) {
    closesocket(list->array[i].socket);
    for (size_t k = i; k < list->length - 1; ++k)
        list->array[k] = list->array[k + 1];
    list->length--;
}

int peer_getPeer(struct PeerList list, SOCKET socket) {
    for (size_t i = 0; i < list.length; ++i) {
        if (list.array[i].socket == socket)
            return i;
    }
    return -1;
}

int peer_ID_getPeer(struct PeerList list, char *c) {
    for (size_t i = 0; i < list.length; ++i) {
        if (strcmp(list.array[i].peerData.id, c) == 0)
            return i;
    }
    return -1;
}

void sendErrorMSG(const char *msg, SOCKET socket) {
    char handshake[DEFAULT_BUFLEN] = "@valid=false&error=";
    strcat(handshake, msg);
    int res = send(socket, handshake, (int) strlen(handshake), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending error message!Disconnecting...");
        printLastError();
        closesocket(socket);
    }
}

int constructNodeData(Map *handshake, const PeerList *list, SOCKET *sock, Node_data *result) {

    if (handshake->pairs == NULL) {
        free(handshake->pairs);
        return -1;
    }

    if (map_isFound(*handshake, "valid") && strcmp("false", map_getValue(*handshake, "valid")) == 0) {
        char error[129];
        sprintf(error, "Peer closed connection! Error: %s\n", map_getValue(*handshake, "error"));
        logger_log(error);
        free(handshake->pairs);
        closesocket(*sock);
        return -1;
    }

    char *id = map_getValue(*handshake, "id");
    if (id != NULL) {
        strcpy(result->id, id);
    } else {
        logger_log("Error: Invalid response!ID not found in handshake.");
        sendErrorMSG("ID_NOT_FOUND", *sock);
        free(handshake->pairs);
        closesocket(*sock);
        return -1;
    }
    char *port_str = map_getValue(*handshake, "port");
    if (port_str != NULL) {
        result->port = atoi(port_str);
    } else {
        logger_log("Error: Invalid response!Port not found in handshake.");
        sendErrorMSG("PORT_NOT_FOUND", *sock);
        free(handshake->pairs);
        closesocket(*sock);
        return -1;
    }
    memset(result->nick, 0, 30);
    char *nickname = map_getValue(*handshake, "nickname");
    if (nickname != NULL) {
        strcpy(result->nick, nickname);
    }
    if (peer_ID_isFound(*list, result->id)) {
        logger_log("Handshake received, but the id sent is taken! Dropping peer...");
        sendErrorMSG("ID_TAKEN", *sock);
        free(handshake->pairs);
        closesocket(*sock);
        return -1;
    }
    return 0;
}

SOCKET initPeer(char *ip, int port, struct sockaddr_in *hint) {
    hint->sin_family = AF_INET;
    hint->sin_port = htons(port);
    inet_pton(AF_INET, ip, &hint->sin_addr);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == SOCKET_ERROR) {
        printLastError();
        closesocket(sock);
        return INVALID_SOCKET;
    }
    int res = connect(sock, (struct sockaddr *) hint, sizeof(*hint));
    if (res == SOCKET_ERROR) {
        printLastError();
        closesocket(sock);
        return INVALID_SOCKET;
    }

    return sock;
}

int connectToReceivedPeers(char *peers, Node_data my, FD_SET *fdSet, PeerList *list) {
    char *rest = peers;
    if (peers != NULL) {
        char *tmp = strtok_s(peers, ",", &rest);
        while (tmp != NULL) {
            char ip1[NI_MAXHOST];
            int port1;
            if (sscanf(tmp, "%[^:]:%d", ip1, &port1) != 2) {
                tmp = strtok_s(NULL, ",", &rest);
                continue;
            }

            peer_ConnetctTo(ip1, port1, list, my, fdSet);
            tmp = strtok_s(NULL, ",", &rest);
        }
    }
    return 0;
}

void constructHandshake(Node_data my, const PeerList *peerList, char *handshake) {
    sprintf(handshake, "@id=%s&port=%d&version=%s", my.id, my.port, P2P_CURRENT_VERSION);

    if (strlen(my.nick) != 0) {
        sprintf(handshake, "%s&nickname=%s",handshake, my.nick);
    }
    if(peerList != NULL) {
        char peers[DEFAULT_BUFLEN] = "&peers=";
        for (size_t i = 0; i < peerList->length; ++i) {
            strcat(peers, peerList->array[i].peerData.ip);
            strcat(peers, ":");

            char port[10];
            sprintf(port, "%d", peerList->array[i].peerData.port);

            strcat(peers, port);
            strcat(peers, ",");
        }
        if (strcmp("&peers=", peers) != 0) {
            //Mivel minden ip után rak egy vesszőt (",") így az utolsó utánit el kell távolítani
            peers[strlen(peers) - 1] = '\0';
            strcat(handshake, peers);
        }
    }
}

void peer_loadPeerList(PeerList *list, Node_data mynode, fd_set *master) {
    FILE *peer_file;
    peer_file = fopen("peers.txt", "r");
    if (peer_file == NULL) {
        logger_log("peers.txt not found!");
        peer_file = fopen("peers.txt", "w");
        fprintf(peer_file, "");

    } else {
        char ip[NI_MAXHOST];
        int port;
        while (fscanf(peer_file, "%[^:]:%d\n", ip, &port) == 2) {
            if (peer_ConnetctTo(ip, port, list, mynode, master) != 0) {
                logger_log("Error while connecting to peer...");
            }
        }

    }
    fclose(peer_file);
}