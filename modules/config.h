//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//


#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "../utility.h"
#include "../lib/map.h"
#include "../lib/debugmalloc/debugmalloc.h"
//! Az alapértelmezett hallgató portszám
#ifdef RANDOM_PORT
#define DEFAULT_PORT "0"
#else
#define DEFAULT_PORT "6327"
#endif
//! Az alapértelmezett hallgató portszám a felhasználói felület számára
#define DEFAULT_INTERFACE_PORT "5081"
//! Az alapértelmezett mappa, ami felhasználói felület számára szükséges fájlokat tárolja
#define DEFAULT_WWW_FOLDER "htdocs/"
//! A program jelenlegi verziószáma
#define P2P_CURRENT_VERSION "1.2"
/*!
 * @brief Betölti a `config.ini` fájlt és létrehoz egy Map struktúrát abból.
 * @return A betöltött file Map struktúrája
 */
Config config_load();
