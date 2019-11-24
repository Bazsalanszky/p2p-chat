//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "debugmalloc/debugmalloc.h"

//! Párok, a map-ek építőlövei
typedef struct Pair { //! A map
    char key[65];
    char value[1024];
} Pair;
//! Egy párokból álló dinamikusan méretezett tömb,ami a kucsok alapján rendezve van a gyorsabb keresés érdekében
typedef struct Map {//! A lista mérete a memóriában
    size_t size;    //! A lista elemeinek száma
    size_t length;  //! Párok tömbje
    Pair *pairs;
} Map, //! Konfigurációt tároló struktóra(Lásd Map)
Config;
/*!
 * @brief Map struktúra inicializálása. Minden struktúra létrehozása után meg kell híni
 * @param[in,out] m Az inicializálandó map mutatója
 */
void map_init(Map *m);
/*!
 * @brief A mapen belül megkeres egy értéket és visszatér egy logikai értékkel az alapján hogy a keresett kulcs létezik-e.
 * @param[in] map A map amiben keressük a kulcsot
 * @param[in] key a keresett kulcs
 * @note A kereséshez bináris keresést alkalmaz
 * @return Logikai érték az alapján hogy a keresett kulcs létezik-e vagy sem.
 */
bool map_isFound(Map map, char *key);
/*!
 * @brief Paraméterként kér egy map struktúrát és egy kukcsot és visszatér a kulcshoz tartozó értékkel
 * @param[in] map A map amiben keressük az értéket
 * @param[in] key A kulcs amihez tartozó értéket keressük
 * @note Ha a megadott kulcs nem létezik akkor NULL-al tér vissza.
 * @return A kulcshoz tartozó érték
 */
char *map_getValue(Map map, char *key);
/*!
 * @brief Hozzáad egy párt a megadott map-hoz.
 * Pelda:
 * \code map_addPair(&map1,map_make_pair("kulcs"),"ertek"); \endcode
 * @param[in] map Mutató a map-ra ahová szeretnénk behelyezni
 * @param[in] pair A pár amit szeretnénk behelyezni a map-ba
 * @note Az elem hozzáadása után a map automatikusan újrarendeződik
 */
void map_addPair(Map *map, Pair pair);
/*!
 * @brief Létrehoz egy pár(pair) struktúrát
 * @param key Kulcs
 * @param value Érték
 * @return Pair,hozzá adható egy map-hez a map_addPair() függvénnyel
 */
Pair map_make_pair(char *key, char *value);

/*!
 * @brief Sorba rendez egy mapot a kulcsok alapján
 * @param map Mutató a rendezendő map-ra
 * @note Új elem hozzáadása esetén automatikusan lefut
 */
void map_sort(Map *map);

/*!
 * @brief Kiírja a megadott map összes kulcsát és a hozzájuk tartozó értéket tabulátorral elválasztva
 * @param map
 * @note Debugoláshoz hasznos
 */
void map_dump(Map map);