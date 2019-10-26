//
// Készítette: Toldi Balázs Ádám
// Dátum: 2019. 10. 20.
//
#include "crypto.h"

#undef APPMACROS_ONLY
#include <openssl/applink.c>


RSA * createRSA(unsigned char * key,int public)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
        }
    if(public)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
        }
    if(rsa == NULL)
    {
        printf( "Failed to create RSA");
    }

    return rsa;
}

RSA *createRSAfromFile(char *file, int pub) {
    FILE * fp = fopen(file,"rb");

    if(fp == NULL)
    {
        printf("Unable to open file %s \n",file);
        return NULL;
    }
    RSA *rsa= RSA_new() ;
    if(pub)
    {
        rsa = PEM_read_RSA_PUBKEY(fp, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_RSAPrivateKey(fp, &rsa,NULL, NULL);
    }
    return rsa;
}



RSA *generate_key() {
    int             ret = 0;
    RSA             *r = NULL;
    BIGNUM          *bne = NULL;
    BIO             *bp_public = NULL, *bp_private = NULL;

    int             bits = 2048;
    unsigned long   e = RSA_F4;

    bne = BN_new();
    ret = BN_set_word(bne,e);
    if(ret != 1){
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        RSA_free(r);
        BN_free(bne);
    }

    r = RSA_new();
    ret = RSA_generate_key_ex(r, bits, bne, NULL);
    if(ret != 1){
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        RSA_free(r);
        BN_free(bne);
    }

    bp_public = BIO_new_file("public.pem", "w+");
    ret = PEM_write_bio_RSAPublicKey(bp_public, r);
    if(ret != 1){
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        RSA_free(r);
        BN_free(bne);
    }

    bp_private = BIO_new_file("private.pem", "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);

    return r;

}

void RSA_getPublicKey(RSA*r,char* pubkey){
    int keylen;

    BIO *bio = BIO_new(BIO_s_mem());
    int res = PEM_write_bio_RSA_PUBKEY(bio,r);
    keylen = BIO_pending(bio);

    int re = BIO_read(bio, pubkey, keylen);
    BIO_free_all(bio);
}

void RSA_getPrivateKey(RSA *r, char *privkey) {
    int keylen;

    BIO *bio = BIO_new(BIO_s_mem());
    int res = PEM_write_bio_RSAPrivateKey(bio,r,NULL,NULL,0,NULL,NULL);
    keylen = BIO_pending(bio);

    int re = BIO_read(bio, privkey, keylen);
    BIO_free_all(bio);
}
int public_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted) {
    RSA * rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,RSA_PKCS1_PADDING);
    return result;
}

int private_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted) {
    RSA * rsa = createRSA(key,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,RSA_PKCS1_PADDING);
    return result;
}

int base64Encode(const unsigned char* input ,size_t len, char** output) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
    BIO_write(bio, input, len);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    *output=bufferPtr->data;


    return 0;
}
size_t calcDecodeLength(const char* b64input) { //Calculates the length of a decoded string
    size_t len = strlen(b64input),
            padding = 0;

    if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
        padding = 2;
    else if (b64input[len-1] == '=') //last char is =
        padding = 1;

    return (len*3)/4 - padding;
}

int base64Decode(const char* input, unsigned char**buffer,size_t* len) { //Decodes a base64 encoded string
    BIO *bio, *b64;

    int decodeLen = calcDecodeLength(input);
    *buffer = (unsigned char*)malloc(decodeLen + 1);
    (*buffer)[decodeLen] = '\0';

    bio = BIO_new_mem_buf(input, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
    *len = BIO_read(bio, *buffer, strlen(input));
    assert(*len == decodeLen); //length should equal decodeLen, else something went horribly wrong
    BIO_free_all(bio);

    return (0); //success
}


