//
// Created by Balazs on 2019. 10. 11..
//
#include "peer.h"

//Kouhai peer
//Amikor mi csatlakozunk egy peerhez
int peer_ConnetctTo(char* ip,int port,peerList* peerList, node_data my,fd_set* fdSet){
    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ip, &hint.sin_addr);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return -1;
    }
    int res = connect(sock, (struct sockaddr*) &hint, sizeof(hint));
    if (res == SOCKET_ERROR) {
        return -1;
    }
    logger_log("Connected to peer!Sending handshake...");
    char handshake[DEFAULT_BUFLEN];
    sprintf(handshake,"@id=%s&port=%d",my.id,my.port);

    if(strlen(my.nick) != 0) {
        char buf[DEFAULT_BUFLEN];
        ZeroMemory(buf,DEFAULT_BUFLEN);
        sprintf(buf, "&nickname=%s",my.nick);
        strcat(handshake,buf);
    }
    res = send(sock,handshake,strlen(handshake),0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending peer list!Disconnecting..." );
        closesocket(sock);
        return -1 ;
    }
    logger_log("Sent!Waiting for response...");
    char buf[DEFAULT_BUFLEN];
    int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
    if (inBytes <= 0) {
        logger_log("Error: Invalid response!");
        closesocket(sock);
        return -1;
    }
    if(buf[0] != '@'){
        logger_log("Error: Invalid response!");
        return -1;
    }
    int len;
    printf("%s\n",buf);
    map m = getHandshakeData(buf);
    node_data node;
    strcpy(node.ip,ip);

    if(map_isFound(m,"valid") && strcmp("false", map_getValue(m,  "valid")) == 0) {
        char error[129];
        sprintf(error,"Peer closed connection! Error: %s\n",map_getValue(m,"error"));
        logger_log(error);
        closesocket(sock);
        return -1;
    }


    if(map_isFound(m,"id")) {
        strcpy(node.id, map_getValue(m,  "id"));
    } else {
        logger_log("Error: Invalid response!ID not found in handshake.");
        return -1;
    }
    if(map_isFound(m,"port")) {
        node.port = atoi(map_getValue(m,  "port"));
    } else {
        logger_log("Error: Invalid response!Port not found in handshake.");
        return -1;
    }

    if(map_isFound(m,"nickname")) {
        strcpy(node.nick, map_getValue(m,  "nickname"));
    }

    Peer p;
    p.peerData = node;
    p.socket = sock;
    p.sockaddr = hint;
    FD_SET(sock,fdSet);
    peer_addTolist(peerList,p);
    //TODO: Connect to recived peers
    if(map_isFound(m,"peers")) {
        char* tmp = strtok(map_getValue(m,"peers"),",");
        while(tmp != NULL){
            char ip[NI_MAXHOST];
             int port;
             sscanf(tmp, "%[^:]:%d", ip, &port);
             if(!peer_isIPfoundInList(*peerList,ip,port))
                 peer_ConnetctTo(ip,port,peerList,my,fdSet);
            tmp = strtok(NULL,",");
        }
    }
    logger_log("Peer validated (%s->%s)!",node.ip,node.id);
    return 0;
}

//Senpai peer
//Amikor a egy peer csatlakozik hozzánk
int peer_HandleConnection(SOCKET listening,peerList *peerList, node_data my,fd_set* fdSet){
    struct sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET sock = accept(listening, (struct sockaddr*)& client, &clientSize);

    char ip[NI_MAXHOST];
    char service[NI_MAXSERV];

    ZeroMemory(ip, NI_MAXHOST);

    inet_ntop(AF_INET, &client.sin_addr, ip, NI_MAXHOST);

    char buf[DEFAULT_BUFLEN];

    int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
    if (inBytes <= 0) {
        closesocket(sock);
        return -1;
    }
    if (buf[0] != '@') {
        closesocket(sock);
        return -1;
    }


    int len;
    map m = getHandshakeData(buf);

    node_data node;
    strcpy(node.ip,ip);
    if(map_isFound(m,"id")) {
        strcpy(node.id, map_getValue(m,  "id"));
    } else {
        logger_log("Error: Invalid response!ID not found in handshake.");
        return -1;
    }
    if(map_isFound(m,"port")) {
        node.port = atoi(map_getValue(m,  "port"));
    } else {
        logger_log("Error: Invalid response!Port not found in handshake.");
        return -1;
    }

    if(map_isFound(m,"nickname")) {
        strcpy(node.nick, map_getValue(m,  "nickname"));
    }
    if(peer_isFoundInList(*peerList,node.id)){
        logger_log("Handshake received, but the id sent is taken! Dropping peer...");
        char handshake[1024] = "@valid=false&error=ID_TAKEN";
        int res = send(sock, handshake, strlen(handshake), 0);
        if (res == SOCKET_ERROR) {
            logger_log("Error sending error message!Disconnecting...");
            closesocket(sock);
            return -1;
        }
        closesocket(sock);
        return -1;
    }
    logger_log("Handshake recived! Sending response!");
    char* handshake = (char*) calloc(DEFAULT_BUFLEN, sizeof(char));
    sprintf(handshake,"@id=%s&port=%d",my.id,my.port);
    if(strlen(my.nick) != 0) {
        ZeroMemory(buf,DEFAULT_BUFLEN);
        sprintf(buf, "&nickname=%s",my.nick);
        strcat(handshake,buf);
    }
    char peers[DEFAULT_BUFLEN] = "&peers=";
    for (int i = 0; i < peerList->length; ++i) {
        strcat(peers,peerList->array[i].peerData.ip);
        strcat(peers,":");

        char port[10];
        sprintf(port, "%d", peerList->array[i].peerData.port);

        strcat(peers,port);
        strcat(peers,",");
    }
    if(strcmp("&peers=",peers) != 0) {
        //Mivel minden ip után rak egy vesszőt (",") így az utolsó utánit el kell távolítani
        peers[strlen(peers) - 1] = '\0';
        strcat(handshake,peers);
    }
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
    peer_addTolist(peerList,p);
    FD_SET(sock,fdSet);

    logger_log("Peer successfully connected from %s with id %s",inet_ntoa(client.sin_addr),node.id);
    return 0;
}

void peer_initList(peerList *list){
        list->size = 0;
        list->length = 0;
        list->array = 0;
}


bool peer_isFoundInList(peerList list,char* id){
    for(int i=0;i < list.length;++i){
        if(strcmp(list.array[i].peerData.id,id)==0) {
            return true;
        }
    }
    return false;
}
bool peer_isIPfoundInList(struct peerList list,char* ip,int port){
    for(int i=0;i < list.length;++i){
        if(strcmp(list.array[i].peerData.ip,ip) == 0 && list.array[i].peerData.port == port) {
            return true;
        }
    }
    return false;
}


void peer_addTolist(peerList *list, struct peer p){
    if (list->length >= list->size)
    {
        assert(list->length == list->size);
        size_t new_size = (list->size + 2) * 2;
        Peer *new_list = realloc(list->array, new_size * sizeof(Peer));
        if (new_list == 0)
            printf("OUT OF MEMORY!");
        list->size = new_size;
        list->array      = new_list;
    }
    list->array[list->length++] = p;
}
void peer_removeFromList(struct peerList* list, int i){

    for (int k=i; k < list->length-1; ++k)
        list->array[k] =list->array[k+1];
    list->length--;
}

int peer_getPeer(struct peerList list,SOCKET socket){
    for (int i = 0; i < list.length; ++i) {
        if(list.array[i].socket == socket)
            return i;
    }
    return  -1;
}

int peer_getIDPeer(struct peerList list, char* c) {
    for (int i = 0; i < list.length; ++i) {
        if(strcmp(list.array[i].peerData.id,c) == 0)
            return i;
    }
    return  -1;
}

Peer peer_getPeerByID(struct peerList list, char *c) {
    Peer res;
    for (int i = 0; i < list.length; ++i) {
        if(strcmp(list.array[i].peerData.id,c) == 0)
            return list.array[i];
    }
    return res;
}
