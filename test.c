//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//
#include "utility.h"


int main(void){
    char contents[] = "@content=0&website=Google";
    int len;
    map *m1 = getHandshakeData(contents,&len);
    if(map_isFound(m1,len,"website")){
        printf("Found ya!\n");
        printf("Value: %s\n",map_getValue(m1,len,"website"));
    }else printf("Ohh fuck...\n");
    return 0;
}