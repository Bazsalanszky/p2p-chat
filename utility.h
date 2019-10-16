//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//

#pragma once
#include "modules/map.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <openssl/md5.h>
#include <ws2tcpip.h>



#define DEFAULT_BUFLEN 4096

char* generateSeed(int len);

map getHandshakeData(char* text);

void md5(char *string, char outputBuffer[33]);

void logger_log(const char* _Format, ...);