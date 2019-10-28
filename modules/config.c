//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//
#include "config.h"

config config_load(){
    config cfg;
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
            ZeroMemory(key, 65);
            ZeroMemory(value, 65);
            ZeroMemory(buf, 256);
        }
    }else{

        char* exampleConf= "#Becenév\n"
                           "#nickname=Pelda\n"
                           "#A program által használt port (Alapértelmezett: 6381)\n"
                           "port=6327\n"
                           "#Letiltja a külső csatlakozást (Alapértelmezett: false)\n"
                           "localmode=false\n"
                           "#Ezne a porton lesz elérhető a felhasználói felület (Alapértelmezett: 5381)\n"
                           "interface-port=5381\n"
                           "#Ebben a mappában vannak tárolva a html fájlok a felhasználói felülethez (Alapértelmezett: htdocs/)\n"
                           "interface-folder=htdocs/\n"
                           "#A felhasználói felület csak ezen a gépen érhető elő (Alapértelmezett: true;Ajánlott)\n"
                           "interface-local=true" ;
        f = fopen("config.ini","w");
        fprintf(f,exampleConf);
        fclose(f);
    }

    return cfg;
}