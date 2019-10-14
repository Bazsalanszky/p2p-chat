//
// Created by Balazs on 2019. 10. 11..
//
#include "peer.h"

//Kouhai peer
//Amikor mi csatlakozunk egy peerhez
int peer_ConnetctTo(char* ip,int port,Peer peerList[],int* peerCount, node_data my,fd_set* fdSet){
    if(*peerCount+1 >= DEFAULT_MAX_PEER_COUNT){
        printf("Couldn't connect,because the max peer count is reached!\n");
        return -1;
    }
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
    printf("Connected to peer!Sending handshake...\n");
    char handshake[DEFAULT_BUFLEN];
    if(strlen(my.nick) == 0)
        sprintf(handshake,"@id=%s&port=%d",my.id,my.port);
    else sprintf(handshake,"@id=%s&port=%d&nickname=",my.id,my.port,my.nick);
    res = send(sock,handshake,strlen(handshake),0);
    if (res == SOCKET_ERROR) {
        printf("Error sending peer list!Disconnecting...\n" );
        closesocket(sock);
        return -1 ;
    }
    printf("Sent!Waiting for response...\n");
    char buf[DEFAULT_BUFLEN];
    int inBytes = recv(sock, buf, DEFAULT_BUFLEN, 0);
    if (inBytes <= 0) {
        printf("Error: Invalid response!\n");
        closesocket(sock);
        return -1;
    }
    if(buf[0] != '@'){
        printf("Error: Invalid response!\n");
        return -1;
    }
    int len;
    map *m = getHandshakeData(buf,&len);
    node_data node;
    strcpy(node.ip,ip);

    if(map_isFound(m,len,"valid") && strcmp("false", map_getValue(m, len, "valid")) == 0) {
        printf("Peer closed connection! Error: %s\n",map_getValue(m,len,"error"));
        closesocket(sock);
        return -1;
    }


    if(map_isFound(m,len,"id")) {
        strcpy(node.id, map_getValue(m, len, "id"));
    } else {
            printf("Error: Invalid response!ID not found in handshake.\n");
            return -1;
    }
    if(map_isFound(m,len,"port")) {
        node.port = atoi(map_getValue(m, len, "port"));
    } else {
            printf("Error: Invalid response!Port not found in handshake.\n");
            return -1;
    }

    if(map_isFound(m,len,"nickname")) {
        strcpy(node.nick, map_getValue(m, len, "nickname"));
    }

    Peer p;
    p.peerData = node;
    p.socket = sock;
    p.sockaddr = hint;
    FD_SET(sock,fdSet);
    peerList = peer_addTolist(peerList,peerCount,p);
    //TODO: Connect to recived peers
    if(map_isFound(m,len,"peers")) {
        char* tmp = strtok(map_getValue(m,len,"peers"),",");
        while(tmp != NULL){
            printf("%s\t%d\n",tmp,*peerCount);
            char ip[NI_MAXHOST];
             int port;
             sscanf(tmp, "%[^:]:%d", ip, &port);
             if(!peer_isIPfoundInList(peerList,*peerCount,ip,port))
                 peer_ConnetctTo(ip,port,peerList,peerCount,my,fdSet);
            tmp = strtok(NULL,",");
        }
    }
    printf("Peer validated (%s->%s)!\n",node.ip,node.id);
    return 0;
}

//Senpai peer
//Amikor a egy peer csatlakozik hozzánk
int peer_HandleConnection(SOCKET listening,Peer peerList[],int* peerCount, node_data my,fd_set* fdSet){
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
    map *m = getHandshakeData(buf,&len);
    node_data node;
    strcpy(node.ip,ip);
    if(map_isFound(m,len,"id")) {
        strcpy(node.id, map_getValue(m, len, "id"));
    } else {
        printf("Error: Invalid response!ID not found in handshake.\n");
        return -1;
    }
    if(map_isFound(m,len,"port")) {
        node.port = atoi(map_getValue(m, len, "port"));
    } else {
        printf("Error: Invalid response!Port not found in handshake.\n");
        return -1;
    }

    if(map_isFound(m,len,"nickname")) {
        strcpy(node.nick, map_getValue(m, len, "nickname"));
    }
    if(peer_isFoundInList(peerList,*peerCount,node.id)){
        printf("Handshake received, but the id sent is taken! Dropping peer...\n");
        char handshake[1024] = "@valid=false&error=ID_TAKEN";
        int res = send(sock, handshake, strlen(handshake), 0);
        if (res == SOCKET_ERROR) {
            printf("Error sending error message!Disconnecting...\n");
            closesocket(sock);
            return -1;
        }
        closesocket(sock);
        return -1;
    }
    printf("Handshake recived! Sending response!\n");
    char handshake[DEFAULT_BUFLEN];
    if(strlen(my.nick) == 0)
        sprintf(handshake,"@id=%s&port=%d",my.id,my.port);
    else sprintf(handshake,"@id=%s&port=%d&nickname=",my.id,my.port,my.nick);

    char peers[DEFAULT_BUFLEN] = "&peers=";
    for (int i = 0; i < *peerCount; ++i) {
        strcat(peers,peerList[i].peerData.ip);
        strcat(peers,":");

        char port[10];
        sprintf(port, "%d", peerList[i].peerData.port);

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
        printf("Error sending handshake!Disconnecting...\n");
        closesocket(sock);
        return -1;
    }
    Peer p;
    p.peerData = node;
    p.socket = sock;
    p.sockaddr = client;
    peerList = peer_addTolist(peerList,peerCount,p);
    FD_SET(sock,fdSet);
    printf("Peer successfully connected from %s whit id %s\n",inet_ntoa(client.sin_addr),node.id);
    return 0;
}

bool peer_isFoundInList(struct peer list[],int peerCount,char* id){
    for(int i=0;i < peerCount;++i){
        if(strcmp(list[i].peerData.id,id)==0) {
            return true;
        }
    }
    return false;
}
bool peer_isIPfoundInList(struct peer list[],int peerCount,char* ip,int port){
    for(int i=0;i < peerCount;++i){
        if(list[i].peerData.ip == ip && list[i].peerData.port == port) {
            return true;
        }
    }
    return false;
}


Peer* peer_addTolist(struct peer list[],int *peerCount, struct peer p){
    Peer *peers = (Peer*) malloc(sizeof(Peer)*((*peerCount)+1));
    for (int j = 0; j < *peerCount; ++j) {
        peers[j] = list[j];
    }
    list[*peerCount] = p;
    *peerCount += 1;
    return peers;
}
Peer* peer_removeFromList(struct peer list[],int *peerCount, int i){
    Peer *peers = (Peer*) malloc(sizeof(Peer)*((*peerCount)-1));
    for (int k=0,j = 0; j < *peerCount; ++j) {
        if(j == i) continue;
        peers[k] = list[j];
        k++;
    }

    *peerCount -=1;
    return peers;
}

int peer_getPeer(struct peer list[],int peerCount,SOCKET socket){
    for (int i = 0; i < peerCount; ++i) {
        if(list[i].socket == socket)
            return i;
    }
    return  -1;
}
