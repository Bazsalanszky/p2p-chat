/*!
 *  @author Toldi Balázs Ádám
 *  @date 2019. 10. 09.
*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "../utility.h"
#include "config.h"
#include "../lib/tcp-listener.h"
//! A különböző gépek adatait tároló struktúra
typedef struct Node_data { //! A node IP címe(IPV4)
    char ip[NI_MAXHOST];    //! A node azonosítója                                                
    char id[33];            //! Felhasználó beceneve
    char nick[30];          //! A port amin a node "hallgató" socket-e fut                        
    int port;
} Node_data;
//! Egy peer adatainak összesítése
typedef struct peer{        //! A peer általános adatait tartalmazó struktúra
    Node_data peerData;     //! A socket, amin a peer csatlakozva van
    SOCKET socket;          //! A socket-et főbb adatait tartalmazó struktúra
    struct sockaddr_in sockaddr;
} Peer;
//! A peereket tartó lista
//! @note Ez egy dinaamikusan méretezett tömb. Használat után fel kell szabadítani a lefoglalt memoriaterületet.
typedef struct PeerList{
    //! A lista által lefoglalt hely
    size_t size;
    //! A listán található elemek száma
    size_t length;
    //! A peerek listájának tömbje
    Peer * array;
} PeerList;

/**
 *  @brief Új kapcsolat létrehozása csatlakozás útján
 *  @param[in] ip a gép IP címe (IPv4) amire csatlakozni szeretnénk
 *  @param[in] port A port amin keresztül csatlakozni szeretnénk
 *  @param[in,out] peerList A lista hová az újdonsült peert helyezni fogjuk
 *  @param[in] myData Az az adatstruktúra ami a saját adatainkat tároljuk
 *  @param[in,out] fdSet Ebben tároljuk a peer sokcet-ét. select() függvényhez szükséges
 *  @note Ebben fut le a kézfogás.
 *  @return Függvény sikerességét jelző egész
 *  @note Ha nem nulla értékkel tér vissza,akkor a csatlakozás sikertelen volt.
 */
int peer_ConnetctTo(char* ip, int port, PeerList* peerList, Node_data myData, fd_set* fdSet);

/*!
 * @brief Kezeli a kívülről érkező csatlakozásokat
 * @param[in] listening A program "hallgató" socket-e. Az accept() függvényhez szükséges.
 * @param[in,out] peerList A lista hová az újdonsült peert helyezni fogjuk
 * @param[in] myData Az az adatstruktúra ami a saját adatainkat tároljuk
 * @param[in,out] fdSet Ebben tároljuk a peer sokcet-ét. select() függvényhez szükséges
 * @note Ebben fut le a kézfogás
 * @return Függvény sikerességét jelző egész
 * @note Ha nem nulla értékkel tér vissza,akkor a csatlakozás sikertelen volt.
 */
int peer_HandleConnection(SOCKET listening, PeerList* peerList, Node_data myData, fd_set* fdSet);
/*!
 * @brief Inicializálja a peerList struktúrát.
 * @param[in,out] list A lista amit inicializálni szereznénk.
 */
void peer_initList(PeerList *list);
/*!
 * @brief Megnézi hogy a peer listában szerepel-e a megadott azonosító
 * @param[in] list A peerek listája
 * @param[in] id A keresett azonosító
 * @return Logikai változó.Ha megtalálta akkor true, ha nem akkor false.
 */
bool peer_ID_isFound(struct PeerList list, char* id);
/*!
 * @brief Megnézi hogy a peer listában szerepel-e a megadott IP cím és port páros.
 * @param[in] list A peerek listája
 * @param[in] ip A keresett IP cím
 * @param[in] port A keresett port
 * @return Logikai változó.Ha megtalálta akkor true, ha nem akkor false.
 */
bool peer_IP_isFound(struct PeerList list, char* ip, int port);
/*!
 * @brief Hozzáad egy elemet a peer listához
 * @param[in,out] list Mutató a PeerList struktúrára,amihez az új elemet szeretnénk felvenni
 * @param[in] peer Hozzáadandó peer
 */
void peer_addTolist(struct PeerList* list, struct peer peer);
/*!
 * @brief Levesz egy elemet egy PeerList -ről.
 * @param[in,out] list Mutató a PeerList struktúrára,amiről le akarunk venni elemet
 * @param[in] index A peer indexe a listában
 */
void peer_removeFromList(struct PeerList *list, int index);
/*!
 * @brief Megkeres egy elemet egy peer listában(PeerList), SOCKET alapján
 * @param[in] list A lista amiben keresni szeretnénk
 * @param[in] socket A keresett socket
 * @return A keresett peer indexe a listában.
 * @note Ha nem találta a megadott SOCKET-et akkor (-1)-el  tér vissza.
 */
int peer_getPeer(struct PeerList list, SOCKET socket);
/*!
 * @brief Megkeres egy elemet egy peer listában(PeerList), azonosító(id) alapján
 * @param[in] list A lista amiben keresni szeretnénk
 * @param[in] id A keresett azonosító
 * @return A keresett peer indexe a listában.
 * @note Ha nem találta a megadott azonosítót akkor (-1)-el  tér vissza.
 */
int peer_ID_getPeer(struct PeerList list, char *id);