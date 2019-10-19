//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//

#pragma once
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Pair{
    char key[65];
    char value[65];
}pair;

typedef struct Map
{
    size_t    size;
    size_t    length;
    pair   *pairs;
} map,config;

void map_init(map *m);

bool map_isFound(map m, char* key);

char* map_getValue(map m, char* key);

void map_addPair(map *m, pair p);

pair map_make_pair(char *key,char *value);

//Debug-hoz hasznos
void map_dump(map m);