//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//

#pragma once
#include "lib/map.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

typedef int SOCKET;

//! (MAXIMUM) Ekkora mérető csomagokat küld el a program alapértelmezetten.
#define DEFAULT_BUFLEN 4096
/*!
 * @brief Generál egy \p len(+1 a lezáró nullának) méretű karaktertömböt,amit feltölt véletlenszerű base58-as karakterekkel
 * @param[in] len A generálandó sztring hossza
 * @return A generált sztring
 */
char* generateSeed(int len);
/*!
 * @brief Értelmez egy megadott csomag sztringjét
 * A csomagok formátuma:
 * <ul>
 * <li>A csomag első karakterének kukac karakternek(@) kell lennie</li>
 * <li>A kulcsokat és a hozzájuk tartozó értékeket egyenlőségjel(=) választja el</li>
 * <li>Az egyes paramétereket AND jel(&) választja el</li>
 * </ul>
 * Példa \code
 * @id=84uzkhv4vMuu2PRD&port=6327&nickname=ToldiBalazs
 * \endcode
 * @param[in] text Az értelmezendő sztring
 * @return Map,ami tartalmazza a kulcsokat és a hozzájuk rendelt értékeket
 * @note A szöveg nem tartalmazhat Entert,tabulátort vagy szóközt.
 */
Map getPacketData(char* text);

/**
 * @brief Kiír egy üzenetet dátummal az elején és sor vége jellel a végén a képernyőre,valamint a log.txt fájlba
 * @param _Format Lásd <a href="http://www.cplusplus.com/reference/cstdio/printf/#parameters">printf</a>
 * @param ...
 */
void logger_log(const char* _Format, ...);