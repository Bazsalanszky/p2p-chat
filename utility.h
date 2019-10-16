//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//
#ifndef P2P_UTILITY_H
#define P2P_UTILITY_H
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <openssl/md5.h>
#include <ws2tcpip.h>
#endif


#define DEFAULT_BUFLEN 1024

char* generateSeed(int len);

typedef struct Map{
    char key[50];
    char value[50];
} map;

map* getHandshakeData(char* text,int *len);

bool map_isFound(map m[],int len, char* key);

char* map_getValue(map m[],int len, char* key);

//Debug-hoz hasznos
void map_dump(map m[],int len);

void md5(char *string, char outputBuffer[33]);

void logger_log(const char* _Format, ...);