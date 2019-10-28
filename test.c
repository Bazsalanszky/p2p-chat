//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//
#include "modules/crypto.h"
#include <openssl/err.h>
#include <ws2tcpip.h>
#include "string.h"


int main(){
    generate_key();
    RSA* r = createRSAfromFile("public.pem",1);

    char plainText[1024/8] = "Hello this is Ravi"; //key length : 2048
    char pubkey[4096];
    RSA_getPublicKey(r,pubkey);
    printf("%s\n",pubkey);

    unsigned char  encrypted[4098]={};
    unsigned char decrypted[4098]={};

    int encrypted_length= public_encrypt(plainText,strlen(plainText),pubkey,encrypted);
    if(encrypted_length == -1)
    {
        //printLastError("Public Encrypt failed ");
        exit(0);
    }
    printf("Encrypted length =%d\n",encrypted_length);








}