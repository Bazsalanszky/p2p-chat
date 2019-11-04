//
// Keszitette: Toldi Balázs Ádám
// Datum: 2019. 10. 28.
//
#pragma once
#include <ws2tcpip.h>
#include <stdbool.h>
#include "../utility.h"

/*!
 * @brief Létrehoz egy Listening socket-et \p port porton
 * @param[in] socket Mutató az inicializálandó socket-re
 * @param[in] port Port amin a socket hallgatni fog
 * @param[in] wildcard Ha az állítás igaz,akkor a <a href="https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo">getaddrinfo()</a> hívása
 * előtt hozzáadódik az <a href="https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo#AI_PASSIVE">AI_PASSIVE</a> flag
 * @return A <a href="https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo">getaddrinfo()</a>
 * által készített struct addrinfo*
 * @note Ha nem sikerül,akkor NULL értékkel tér vissza
 */
struct addrinfo* tcp_createIPv4Socket(SOCKET *socket,int port,bool wildcard);
/*!
 * @brief
 * @param s
 * @param addr
 * @param conn_count
 * @return
 */
int tcp_bindnlisten(SOCKET s,struct addrinfo* addr,int conn_count);
struct sockaddr_in tcp_getAddr_in(SOCKET s);
int tcp_getSockPort(SOCKET s);