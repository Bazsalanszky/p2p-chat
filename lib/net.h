//
// Keszitette: Toldi Balázs Ádám
// Datum: 2019. 11. 05.
//
#pragma once
#if defined(__linux__) || defined(__CYGWIN__)
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
//Mappákhoz
#include <sys/stat.h>
//Winsock2 kompatiblitás érdekében
#ifndef SOCKET_ERROR
    #define SOCKET_ERROR -1
#endif
#ifndef INVALID_SOCKET
    #define INVALID_SOCKET -1
#endif
//Ez winsock2-ben definiálva van (ott elvileg unsigned long long int-ként)
typedef int SOCKET;
#elif _WIN32
#include <ws2tcpip.h>
#endif