//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//
#include "config.h"

config config_load(){
    config cfg;
    map_init(&cfg);
    FILE * f;
    f = fopen("config.txt","r");
    if(f != NULL) {
        char key[65], value[65];
        while (fscanf(f, "%[^=]=%s\n", key, value) != EOF) {
            //Comment in config
            if (key[0] == '#') continue;
            map_addPair(&cfg, map_make_pair(key, value));
            ZeroMemory(key, 65);
            ZeroMemory(value, 65);
        }
    }
    return cfg;
}