//
// Created by Balazs Tolid on 2019. 10. 09..
//
#include <ws2tcpip.h>
#include "utility.h"

#define DEFAULT_MAX_PEER_COUNT 64

typedef struct Node_data {
    char ip[16];
    char id[65];
    char nick[30];
    int port;
} node_data;

typedef struct peer{
    node_data peerData;
    SOCKET socket;
    struct sockaddr_in sockaddr;
} Peer;

Peer peerList[64];

//TODO: Create peerlist

//Kouhai peer
int peer_ConnetctTo(char* ip,int port,Peer peerList[],int *peerCount, node_data my);
//Senpai peer
int peer_HandleConnection(SOCKET listening,Peer peerList[],int *peerCount, node_data my);

bool peer_isFoundInList(struct peer list[],int peerCount,char* id);