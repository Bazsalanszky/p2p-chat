//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11..
//
#include "modules/crypto.h"
#include <openssl/err.h>
#include "string.h"



void printLastError(char *msg)
{
    char * err = malloc(130);;
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n",msg, err);
    free(err);
}

int main(){
    char* h = "asd",hex[40],out[10];
    hexEncode(h,hex);
    hexDecode(hex,out);
    printf("%s,%s\n",hex,out);
    RSA* r = createRSAfromFile("public.pem",1);
    char plainText[1024/8] = "Hello this is Ravi"; //key length : 2048
    char pubkey[1024];
    RSA_getPublicKey(r,pubkey);
    printf("%s\n",pubkey);

    unsigned char  encrypted[4098]={};
    unsigned char decrypted[4098]={};

    int encrypted_length= public_encrypt(plainText,strlen(plainText),pubkey,encrypted);
    if(encrypted_length == -1)
    {
        printLastError("Public Encrypt failed ");
        exit(0);
    }
    printf("Encrypted length =%d\n",encrypted_length);








}