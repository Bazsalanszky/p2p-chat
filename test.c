//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//
#include "utility.h"

char* webio_getFiletype(char* filename){
    char *type = (char*) malloc(10);
    char* tmp = strtok(filename,".");
    while(tmp != NULL){
        type =tmp;
        tmp = strtok(NULL,".");
    }
    return type;
}


int main(void){
    char contents[] = "The quick brown fox jumps over the lazy dog";
    static unsigned char buffer[33];

    md5(contents, buffer);

    printf("%s %d\n", buffer,strlen(buffer));
    char txt[] = "Hello.world.txt";
    printf("%s",webio_getFiletype(txt));
    return 0;
}