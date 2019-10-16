//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//
// Ebben a file-ban olyan függvényeket találunk, amelyeket sok helyen fel lehet használni vagy máshová nem illeszkedne
//

#include "utility.h"

char* generateSeed(int len){
    static const char base58[] = "123456789"
                                 "ABCDEFGHJKLMNPQRSTUVWXYZ"
                                 "abcdefghijkmnopqrstuvwxyz";
    srand(time(0));
    char *result = malloc(sizeof(char) * len+1);
    for(int i = 0;i<len;i++){
        result[i] = base58[rand() % 58];
    }
    result[len] = '\0';
    return result;
}
map getHandshakeData(char* text){
    map result;
    map_init(&result);
    if (text[0] == '@')
        memmove(text, text+1, strlen(text));
    int count = 0;
    for(int i = 0;text[i] != '\0';++i){
        if(text[i] == '&')
            count++;
    }
    int i =0;
    const char c[2] = "&";
    char *tmp;
    tmp = strtok (text,c);

    while (tmp != NULL && i <count+1)
    {
        char key[65];
        char value[65];
        sscanf(tmp,"%[^=]=%s",key,value);
        map_addPair(&result,map_make_pair(key,value));
        ++i;
        tmp = strtok (NULL, "&");
    }
    return result;
}

void md5(char *string, char outputBuffer[33]){
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX sha256;
    MD5_Init(&sha256);
    MD5_Update(&sha256, string, strlen(string));
    MD5_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

void logger_log(const char* _Format, ...){
    FILE * fp;
    fp  = fopen("log.txt","a");
    time_t timer;
    char buf[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buf, 26, "%Y.%m.%d. %H:%M:%S", tm_info);
    char string[513];
    printf("[%s]\t",buf);
    fprintf(fp,"[%s]\t",buf);
    va_list args;
    va_start (args, _Format);
    vprintf(_Format,args);
    vfprintf(fp,_Format,args);
    va_end(args);
    fprintf(fp,"\n");
    printf("\n");

    fclose(fp);
}