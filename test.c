//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//
#include "utility.h"



int main(void){
    char contents[] = "The quick brown fox jumps over the lazy dog";
    static unsigned char buffer[33];

    md5(contents, buffer);

    printf("%s %d\n", buffer,strlen(buffer));
    return 0;
}