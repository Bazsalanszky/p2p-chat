//
// Created by Balazs Tolid on 2019. 10. 09..
//
#pragma once
#include "utility.h"

#define DEFAULT_MAX_PEER_COUNT 64

typedef struct Node_data {
    char ip[NI_MAXHOST];
    char id[MD5_DIGEST_LENGTH+1];
    char nick[30];
    int port;
} node_data;

typedef struct peer{
    node_data peerData;
    SOCKET socket;
    struct sockaddr_in sockaddr;
} Peer;

typedef struct peerList{
    size_t size;    // A lista által lefoglalt hely
    size_t length;  // A listán található elemek száma
    Peer * array;    // A lista
}peerList;

//Kouhai peer
int peer_ConnetctTo(char* ip,int port,peerList* peerList, node_data my,fd_set* fdSet);
//Senpai peer
int peer_HandleConnection(SOCKET listening,peerList* peerList, node_data my,fd_set* fdSet);

void peer_initList(peerList *list);

bool peer_isFoundInList(struct peerList list,char* id);
bool peer_isIPfoundInList(struct peerList list,char* ip,int port);
void peer_addTolist(struct peerList* list, struct peer p);

void peer_removeFromList(struct peerList *list, int i);

int peer_getPeer(struct peerList list,SOCKET socket);
int peer_getIDPeer(struct peerList list,char *c);

