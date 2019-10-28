//
// Készítette: Toldi Balázs Ádám
// Dátum: 2019. 10. 20.
//

#pragma once
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "../utility.h"



RSA* createRSA(unsigned char * key,int public);
RSA* createRSAfromFile(char* filename,int pub);
RSA* generate_key();
void RSA_getPublicKey(RSA*r,char* pubkey);
void RSA_getPrivateKey(RSA*r,char* privkey);

int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);
int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);

int base64Encode(const unsigned char* input ,size_t len, char** outpub);
size_t calcDecodeLength(const char* b64input);
int base64Decode(const char* input, unsigned char**buffer,size_t* len);
void printOpenSSLError(char *msg);