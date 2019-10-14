//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14..
//
#include "webio.h"

#ifdef RANDOM_PORT;
#define DEFAULT_INTERFACE_PORT "0"
#else
#define DEFAULT_INTERFACE_PORT "5081"
#endif


int webio_create(int port,char* folder,WebIO *webIo){
    struct addrinfo hint = {};
    struct addrinfo *result = NULL;
    WebIO wio;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_INET;
    //TODO: Disable this in local modeó
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    //TODO: Use config to determine port
    int res = getaddrinfo(NULL, DEFAULT_INTERFACE_PORT, &hint, &result);
    if (res != 0) {
        printf("Error creating address information! Error code: %d", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    //Creating listening socket
    SOCKET listening = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listening == INVALID_SOCKET) {
        printf("Error creating socket! Error: %d", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    //Binding the socket
    res = bind(listening, result->ai_addr, result->ai_addrlen);
    if (res == SOCKET_ERROR) {
        printf("Error binding socket! Error: %d", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }

    freeaddrinfo(result);

    //TODO: Set max connection count in config
    res = listen(listening, SOMAXCONN);
    if (res == -1) {
        printf("Error starting listening! Error: %d", WSAGetLastError());
        closesocket(listening);
        WSACleanup();
        return -1;
    }
    //Ez alapvetően akkor hasznos amikor a port 0-ra van állítva, azaz akkor amikor a rendszer random választ egyet.
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(listening, (struct sockaddr *) &sin, &len) == -1) {
        printf("Error at getsockname!Error code: %d", WSAGetLastError());
        return -1;
    }
    wio.sockaddr = sin;
    wio.socket= listening;
    strcpy(wio.folder,folder);
    *webIo = wio;
    return 0;
}

int webio_handleRequest(WebIO wio){
    SOCKET client = accept(wio.socket,NULL,NULL);
    char buf[8192];
    int res = recv(client,buf,8192,0);
    if(res <=0){
        printf("Error with web interface!\n");
        closesocket(client);
        return -1;
    }
    //printf("%s\n",buf);
    char response[] ="HTTP/1.1 200 OK "
                     "Content-Encoding: gzip\r\n"
                     "Content-Language: en\r\n"
                     "Content-Type: text/html\r\n\r\n"
                     "<h1>Hello there!</h1>";
    res = send(client,response,strlen(response),0);
    if(res == SOCKET_ERROR){
        printf("Sending failed!");
        return -1;
    }
    shutdown(client,SD_SEND);

}
