//
// Készítette: Toldi Balázs Ádám
// Dátum: 2019. 10. 20.
//

#pragma once

#include <openssl/rsa.h>
#include <openssl/pem.h>



RSA * createRSA(unsigned char * key,int public);
RSA* createRSAfromFile(char* filename,int pub);
RSA* generate_key();

int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);
int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);