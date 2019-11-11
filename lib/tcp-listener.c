//
// Keszitette: Toldi Balázs Ádám
// Datum: 2019. 10. 28.
//

#include "tcp-listener.h"

struct addrinfo* tcp_createIPv4Socket(SOCKET *s,int port,bool wildcard){
    struct addrinfo hint;
    struct addrinfo *result = NULL;

    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_INET;
    if(wildcard)
        hint.ai_flags = AI_PASSIVE;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    //TODO: Use Config to determine port
    char sport[10];
    sprintf( sport, "%d", port);
    int res = getaddrinfo(NULL, sport, &hint, &result);
    if (res != 0) {
        logger_log("Error creating address information! Error code: %d", errno);
        return NULL;
    }

    //Creating listening socket
    *s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (*s == INVALID_SOCKET) {
        logger_log("Error creating socket! Error: %d", errno);
        return NULL;
    }
    return result;
}

int tcp_bindnlisten(SOCKET s,struct addrinfo* addr,int conn_count){
    int res = bind(s, addr->ai_addr, addr->ai_addrlen);
    if (res == SOCKET_ERROR) {
        logger_log("Error binding socket!");
        int r = errno;
        switch(r){
            default:
                logger_log("Error: %s",strerror(errno));
                break;
        }
        freeaddrinfo(addr);
        return 1;
    }

    freeaddrinfo(addr);

    res = listen(s, conn_count);
    if (res == -1) {
        logger_log("Error starting listening! Error: %d", errno);
        closesocket(s);
        return 2;
    }
    return 0;
}

struct sockaddr_in tcp_getAddr_in(SOCKET s) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(s, (struct sockaddr *) &sin, &len) == -1) {
        logger_log("Error at getsockname!Error code: %d", errno);
        closesocket(s);
        exit(1);
    }
    return sin;
}
int tcp_getSockPort(SOCKET s){
    struct sockaddr_in sin = tcp_getAddr_in(s);
    return ntohs(sin.sin_port);
}
#if defined(__linux__) || defined(__CYGWIN__)
int closesocket(SOCKET s) {
        return close(s);
}
#endif