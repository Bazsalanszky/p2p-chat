//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//


#pragma once
#include "../utility.h"
#include "../lib/map.h"
#ifdef RANDOM_PORT
#define DEFAULT_PORT "0"
#else
#define DEFAULT_PORT "6327"
#endif
#define DEFAULT_INTERFACE_PORT "5081"
#define DEFAULT_WWW_FOLDER "htdocs/"
/*!
 * @brief Betölti a `config.ini` fájlt és létrehoz egy Map struktúrát abból.
 * @return A betöltött file Map struktúrája
 */
Config config_load();
