//
// Keszitette: Toldi Balázs Ádám
// Datum: 2019. 10. 28.
//
#pragma once
#include "../utility.h"


#if defined(__linux__) || defined(__CYGWIN__)
int closesocket(SOCKET s);
#endif


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
 * @brief Egy socketet hozzárendel egy socket-címstruktúrához, azaz egy adott helyi portszámhoz és IP-címhez,valamint hallgató állapotba helyezi.
 * @param[in,out] socket A socket amin a műveletet végre akarjuk hajtani
 * @param[in] addr egy socket-címstruktúra
 * @param[in] conn_count A maximum kapcsolatok száma (<a href="https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen#parameters">SOMAXCONN</a>)
 * @return A művelet sikerességét jelző szám kód
 * <table>
 * <tr><th>Kód</th><th>Jelentése</th></tr>
 * <tr><td>0</td><td>A művelet sikeres volt</td></tr>
 * <tr><td>1</td><td>A bind() függvény futása közben hiba lépett fel</td></tr>
 * <tr><td>2</td><td>A listen() függvény futása közben hiba lépett fel</td></tr>
 * </table>
 */
int tcp_bindnlisten(SOCKET socket,struct addrinfo* addr,int conn_count);
/*!
 * @brief Kér egy szoketet és vissza adja a hozzá tartozó socket-címstruktúrát
 * @param[in] socket A socket, aminek a címstruktúráját keressük
 * @return A socket-címstruktúrája
 */
struct sockaddr_in tcp_getAddr_in(SOCKET socket);
/*!
 * @brief Visszaadja egy listening socket portszámát
 * @param[in] socket
 * @return A socket portszáma
 */
int tcp_getSockPort(SOCKET socket);