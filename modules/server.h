//
// Készítette: Toldi Balázs Ádám
// Dátum: 2019. 11. 07.
//
#pragma once
#include "peer.h"
#include "webio.h"
#include "../lib/debugmalloc/debugmalloc.h"
/*!
 * @brief Betölti és csatlakozik a `peer.txt` fájlban megadott peerekhez
 * @param[in,out] list A peerek listájának mutatója
 * @param[in] mynode Az az adatstruktúra amiben a saját adatainkat tároljuk
 * @param[in,out] master Ebben tároljuk a peer sokcet-ét. select() függvényhez szükséges
 */
void peer_loadPeerList(PeerList *list,Node_data mynode,fd_set * master);
/*!
 * @brief Generál vagy betölt egy azonosítót.
 * Ha nem létezik a `seed.txt` fájl akkor generál egy 16 karakterből álló sztringet és elmenti az az újonnan létrehozott `seed.txt` fájéba.
 * Ha létezik,akkor onnan betölti azt.
 * @param[out] output Az azonosító
 */
void getSeed(char*output);
/*!
 * @brief Létrehozza azt az adatstruktúra amiben a saját adatainkat tároljuk.
 * @param[in] cfg A program konfigurációja
 * @return A létrehozott Node_data struktúra
 */
Node_data construct_Mynodedata(Config cfg);
/*!
 * @brief Ez a függvény a program "lelke".Ezt a program inicializálása után hívja meg.
 * Ez kezeli a bejövő csomagokat,kapcsolatokat.
 * @param[in] listening A socket amin a program fut.
 * @param[in,out] master Ebben tároljuk a peer sokcet-ét. select() függvényhez szükséges
 * @param[in] webIo A felhasználói felület adatstruktúrája
 * @param[in] list A peerek listája
 * @param[in] mynode Az az adatstruktúra amiben a saját adatainkat tároljuk
 */
void serverThread(SOCKET listening, fd_set* master, WebIO webIo, PeerList list,Node_data mynode);