//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//
//#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#define PORT 8888
#define MAX_LINE 2048
#define LISTENQ 20


int main(int argc , char **argv)
{
    char buf[10] = "lma";
    char buf2[10] = "lma";

    sprintf(buf,"%c%s",'a',buf2);
    printf("%s",buf);
}
