//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 11.
//
// Ebben a file-ban olyan függvényeket találunk, amelyeket sok helyen fel lehet használni vagy máshová nem illeszkedne
//
#ifndef CRT_SECURE_NO_WARNINGS
#define CRT_SECURE_NO_WARNINGS
#endif
#include "utility.h"

char* generateSeed(int len){
    static const char base58[] = "123456789"
                                 "ABCDEFGHJKLMNPQRSTUVWXYZ"
                                 "abcdefghijkmnopqrstuvwxyz";
    srand((unsigned int) time(NULL));
    char *result = malloc(sizeof(char) * (len+1));
    for(int i = 0;i<len-1;i++){
        result[i] = base58[rand() % 58];
    }
    result[len] = '\0';
    return result;
}
Map getPacketData(char* text){

    Map result;
    map_init(&result);
    if (text[0] == '@')
        memmove(text, text+1, strlen(text));
    int count = 0;
    for(int i = 0;text[i] != '\0';++i){
        if(text[i] == '&')
            count++;
    }
    int i =0;
    char *tmp;
    tmp = strtok (text,"&");

    while (tmp != NULL && i <count+1)
    {
        char key[65];
        char value[1024];
        int r = sscanf(tmp,"%[^=]=%s",key,value);
		if(r == 2)
			map_addPair(&result,map_make_pair(key,value));
		else if(r == 1)
			map_addPair(&result, map_make_pair(key, "true"));
		else {
			tmp = strtok(NULL, "&");
			continue;
		}
		i++;
        tmp = strtok (NULL, "&");
    }
    return result;
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
    printf("[%s]\t",buf);
    fprintf(fp,"[%s]\t",buf);
    va_list args;
    va_start (args, _Format);
    vprintf(_Format,args);
    va_end(args);
    va_start (args, _Format);
    vfprintf(fp,_Format,args);
    va_end(args);
    fprintf(fp,"\n");
    printf("\n");

    fclose(fp);
}