//
// Created by Balazs Tolid on 2019. 10. 09..
//
#include "utility.h"

#define DEFAULT_MAX_PEER_COUNT 64

typedef struct Node_data {
    char ip[NI_MAXHOST];
    char id[MD5_DIGEST_LENGTH];
    char nick[30];
    int port;
} node_data;

typedef struct peer{
    node_data peerData;
    SOCKET socket;
    struct sockaddr_in sockaddr;
} Peer;


//TODO: Create peerlist

//Kouhai peer
int peer_ConnetctTo(char* ip,int port,Peer peerList[],int *peerCount, node_data my,fd_set* fdSet);
//Senpai peer
int peer_HandleConnection(SOCKET listening,Peer peerList[],int *peerCount, node_data my,fd_set* fdSet);

bool peer_isFoundInList(struct peer list[],int peerCount,char* id);
bool peer_isIPfoundInList(struct peer list[],int peerCount,char* ip,int port);
Peer* peer_addTolist(struct peer list[],int *peerCount, struct peer p);

Peer* peer_removeFromList(struct peer list[],int *peerCount, int i);

int peer_getPeer(struct peer list[],int peerCount,SOCKET socket);

