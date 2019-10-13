//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_BUFLEN 1024

char* generateSeed(int len);

typedef struct Map{
    char key[50];
    char value[50];
} map;

map* getHandshakeData(char* text,int *len);

bool map_isFound(map m[],int len, char* key);

char* map_getValue(map m[],int len, char* key);