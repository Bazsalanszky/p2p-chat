//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 16.
//
#include "config.h"

Config config_load(){
    Config cfg;
    map_init(&cfg);
    FILE * f;
    f = fopen("Config.ini","r");
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
        f = fopen("Config.ini","w");
        fprintf(f,exampleConf);
        fclose(f);
    }

    return cfg;
}