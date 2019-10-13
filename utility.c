//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//

#include "utility.h"

char* generateSeed(int len){
    static const char base58[] = "123456789"
                                 "ABCDEFGHJKLMNPQRSTUVWXYZ"
                                 "abcdefghijkmnopqrstuvwxyz";
    srand(time(0));
    char *result = malloc(sizeof(char) * 512);
    for(int i = 0;i<len;i++){
        result[i] = base58[rand() % 58];
    }
    result[len] = '\0';
    return result;
}
map* getHandshakeData(char* text,int* len){
    if (text[0] == '\n')
        memmove(text, text+1, strlen(text));
    int count = 0;
    for(int i = 0;text[i] != '\0';++i){
        if(text[i] == '&')
            count++;
    }
    *len = count+1;
    map * m = (map*) malloc(sizeof(map)*(*len));
    int i =0;
    const char c[2] = "&";
    char *tmp;
    tmp = strtok (text,c);

    while (tmp != NULL && i <count+1)
    {
        char key[50];
        char value[50];
        sscanf(tmp,"%[^=]=%s",key,value);
        strcpy(m[i].key,key);
        strcpy(m[i].value,value);
        ++i;
        tmp = strtok (NULL, "&");
    }
    return  m;
}
bool map_isFound(map map[],int len, char* key){
    for (int i = 0; i < len; ++i) {
        if(strcmp(map[i].key,key) == 0)
            return true;
    }
    return false;
}

char* map_getValue(map m[],int len, char* key){
    for (int i = 0; i < len; ++i) {
        if(strcmp(m[i].key,key) == 0)
            return m[i].value;
    }
    return NULL;
}