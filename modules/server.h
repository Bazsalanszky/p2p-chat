//
// Készítette: Toldi Balázs Ádám
// Dátum: 2019. 11. 07.
//
#pragma once
#include "peer.h"
#include "webio.h"
/*!
 *
 * @param list
 * @param mynode
 * @param master
 */
void peer_loadPeerList(PeerList *list,Node_data mynode,fd_set * master);
/*!
 *
 * @param[out] output
 */
void getSeed(char*output);
/*!
 *
 * @param[in] cfg
 * @return
 */
Node_data construct_Mynodedata(Config cfg);
/*!
 *
 * @param listening
 * @param master
 * @param webIo
 * @param list
 * @param mynode
 */
void serverThread(SOCKET listening, fd_set* master, WebIO webIo, PeerList list,Node_data mynode);