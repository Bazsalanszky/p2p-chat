//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//
//#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/map.h"
#include "lib/debugmalloc/debugmalloc.h"

int main(int argc , char **argv)
{
    Map m;
    map_init(&m);
    map_addPair(&m,map_make_pair("Test","1"));
    //map_dump(m);
    free(m.pairs);
    return 0;
}
