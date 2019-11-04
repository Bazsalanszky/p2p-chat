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
        logger_log("Error creating address information! Error code: %d", WSAGetLastError());
        WSACleanup();
        return NULL;
    }

    //Creating listening socket
    *s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (*s == INVALID_SOCKET) {
        logger_log("Error creating socket! Error: %d", WSAGetLastError());
        WSACleanup();
        return NULL;
    }
    return result;
}

int tcp_bindnlisten(SOCKET s,struct addrinfo* addr,int conn_count){
    int res = bind(s, addr->ai_addr, addr->ai_addrlen);
    if (res == SOCKET_ERROR) {
        logger_log("Error binding socket!");
        int r = WSAGetLastError();
        switch(r){
            case WSAENETDOWN:
                logger_log("The network subsystem has failed.");
                break;
            case WSAEADDRINUSE:
                logger_log("Port already in use.");
                break;
            case WSAEACCES:
                logger_log("An attempt was made to access a socket in a way forbidden by its access permissions.");
                break;
            case WSAEADDRNOTAVAIL:
                logger_log("The requested address is not valid in its context.");
                break;
            case WSAEFAULT:
                logger_log("The system detected an invalid pointer address in attempting to use a pointer argument in a call.");
                break;
            case WSAEINPROGRESS:
                logger_log("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                break;
            case WSAEINVAL:
                logger_log("This socket is already bound to an address.");
                break;
            case WSAENOBUFS:
                logger_log("Not enough buffers are available or there are too many connections!");
                break;
            default:
                logger_log("Error: %d",WSAGetLastError());
                break;
        }
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addr);

    res = listen(s, conn_count);
    if (res == -1) {
        logger_log("Error starting listening! Error: %d", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 2;
    }
    return 0;
}

struct sockaddr_in tcp_getAddr_in(SOCKET s) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(s, (struct sockaddr *) &sin, &len) == -1) {
        logger_log("Error at getsockname!Error code: %d", WSAGetLastError());
        closesocket(s);
        exit(1);
    }
    return sin;
}
int tcp_getSockPort(SOCKET s){
    struct sockaddr_in sin = tcp_getAddr_in(s);
    return ntohs(sin.sin_port);
}