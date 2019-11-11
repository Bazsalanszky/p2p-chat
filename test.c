//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//
//#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc , char **argv)
{
    char buf[10] = "lma";
    char buf2[10] = "lma";

    sprintf(buf,"%c%s",'a',buf2);
    printf("%s",buf);
}
