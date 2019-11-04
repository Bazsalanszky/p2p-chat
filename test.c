//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

void hozzad(char* sz,const char* cucc) {
	strcat(sz,cucc);
}

int main(){
	char content[8192] = "sajt";
	hozzad(content, "malac");
    printf("%s",content);
    return 0;

}