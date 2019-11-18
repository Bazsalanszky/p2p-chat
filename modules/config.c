//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//
#include "config.h"

Config config_load(){
    Config cfg;
    map_init(&cfg);
    FILE * f;
    f = fopen("config.ini","r");
    if(f != NULL) {
        char buf[256], key[65], value[65];
        while (fgets(buf, sizeof(buf), f) != NULL)
        {
            if (buf[0] == '#') continue;
            sscanf(buf, "%[^=]=%s\n", key, value);
            map_addPair(&cfg, map_make_pair(key, value));
            memset(key,0, 65);
            memset(value,0, 65);
            memset(buf,0, 256);
        }
    }else{


        f = fopen("config.ini","w");
        fprintf(f,"#Becenév\n"
                  "#nickname=Pelda\n"
                  "#A program által használt port (Alapértelmezett: %s)\n"
                  "port=%s\n"
                  "#Letiltja a külső csatlakozást (Alapértelmezett: false)\n"
                  "localmode=false\n"
                  "#Ezne a porton lesz elérhető a felhasználói felület (Alapértelmezett: %s)\n"
                  "interface-port=%s\n"
                  "#Ebben a mappában vannak tárolva a html fájlok a felhasználói felülethez (Alapértelmezett: %s)\n"
                  "interface-folder=%s\n"
                  "#A felhasználói felület csak ezen a gépen érhető elő (Alapértelmezett: true;Ajánlott)\n"
                  "interface-local=true",DEFAULT_PORT,DEFAULT_PORT,DEFAULT_INTERFACE_PORT,DEFAULT_INTERFACE_PORT,DEFAULT_WWW_FOLDER,DEFAULT_WWW_FOLDER);
        fclose(f);
    }

    return cfg;
}