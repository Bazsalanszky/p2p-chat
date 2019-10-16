//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14..
//
#include "webio.h"

#ifdef RANDOM_PORT
#define DEFAULT_INTERFACE_PORT "0"
#else
#define DEFAULT_INTERFACE_PORT "5081"
#endif


int webio_create(int port,char* folder,struct Node_data myData,WebIO *webIo){
    struct addrinfo hint = {};
    struct addrinfo *result = NULL;
    WebIO wio;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_INET;
    //TODO: Disable this in local modeó
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    //TODO: Use config to determine port
    int res = getaddrinfo(NULL, DEFAULT_INTERFACE_PORT, &hint, &result);
    if (res != 0) {
        char error[129];
        sprintf(error,"Error creating address information!Error code: %d", WSAGetLastError());
        logger_log(error);
        WSACleanup();
        return -1;
    }

    //Creating listening socket
    SOCKET listening = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listening == INVALID_SOCKET) {
        char error[129];
        sprintf(error,"Error creating socket!Error code: %d", WSAGetLastError());
        logger_log(error);
        WSACleanup();
        return -1;
    }

    //Binding the socket
    res = bind(listening, result->ai_addr, result->ai_addrlen);
    if (res == SOCKET_ERROR) {
        char error[129];
        sprintf(error,"Error at binding!Error code: %d", WSAGetLastError());
        logger_log(error);
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }

    freeaddrinfo(result);

    //TODO: Set max connection count in config
    res = listen(listening, SOMAXCONN);
    if (res == -1) {
        char error[129];
        sprintf(error,"Error at starting listening!Error code: %d", WSAGetLastError());
        logger_log(error);
        WSACleanup();
        return -1;
    }
    //Ez alapvetően akkor hasznos amikor a port 0-ra van állítva, azaz akkor amikor a rendszer random választ egyet.
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(listening, (struct sockaddr *) &sin, &len) == -1) {
        char error[129];
        sprintf(error,"Error at getsockname!Error code: %d", WSAGetLastError());
        logger_log(error);
        return -1;
    }
    wio.sockaddr = sin;
    wio.socket= listening;
    wio.nodeData = myData;
    strcpy(wio.folder,folder);
    *webIo = wio;
    return 0;
}

int webio_handleRequest(WebIO wio,Peer list[],int peerCount){
    SOCKET client = accept(wio.socket,NULL,NULL);
    char buf[8192];
    int res = recv(client,buf,8192,0);
    if(res <=0){
        logger_log("Error with web interface!");
        closesocket(client);
        return -1;
    }
    char req[10];
    sscanf(buf,"%s",req);

    if(strcmp(req,"GET") == 0) {
        char file[50];
        sscanf(buf, "%*s %s", file);
        res = webio_handleGETrequest(client,wio,file, list, peerCount);
    }else if(strcmp(req,"POST") == 0)
        res = webio_handlePOSTrequest(client,wio, list,peerCount);
    else
        res = -1;
    return res;

}
char* webio_getMIMEtype(char* filename){
    char* ext = webio_getFiletype(filename);
    if (ext == "html")
        return "text/html";
    else if (ext == "json")
        return "application/json";
    else if (ext == "js")
        return "application/javascript";
    else if (ext == "css")
        return "text/css";
    else if (ext == "jpg")
        return "image/jpeg";
    else if (ext == "png")
        return "image/png";
    else if (ext == "ico")
        return "image/ico";
    else if (ext == "xml")
        return "text/xml";
    else if (ext == "zip")
        return "application/zip";
    else return "text/plain";
}

char* webio_getFiletype(char* filename){
    char *type = (char*) malloc(10);
    char* tmp = strtok(filename,".");
    while(tmp != NULL){
        type =tmp;
        tmp = strtok(NULL,".");
    }
    return type;
}
int webio_handleGETrequest(SOCKET client,WebIO wio,char* file,Peer list[],int peerCount){

    char buf[8192];
    sscanf(buf,"%*s %s",file);

    char path[129];

    ZeroMemory(path,sizeof(path));
    strcat(path,wio.folder);
    char *response = (char *) malloc(sizeof(char) * 8192);
    if(strcmp(file,"/") == 0) {
        strcat(response, "HTTP/1.1 200 OK "
                         "Content-Encoding: gzip\r\n"
                         "Content-Language: en\r\n"
                         "Content-Type: text/html\r\n\r\n"
        );
        strcat(response,getIndex(wio.folder,list,peerCount));
    }else {
        strcat(path, file);

        FILE *fp;
        fp = fopen(path, "r");

        if (fp != NULL) {
            char *content = (char *) malloc(sizeof(char) * 16384);
            ZeroMemory(content, 16384);
            char buf[4096];
            int counter = 0;
            //Ez így működik, de nagyon lassú.
            //TODO: Ki kéne ezt javítani
            while (fgets(buf, 4096, fp) != NULL) {
                int len = strlen(content);
                strcat(content, buf);
                counter += 4096;
                if (counter >= len) {
                    content = realloc(content, counter + 16384);
                }
            }
            fclose(fp);
            response = (char *) malloc(sizeof(char) * counter + 1024);
            ZeroMemory(response, counter);
            strcat(response, "HTTP/1.1 200 OK "
                             "Content-Encoding: gzip\r\n"
                             "Content-Language: en\r\n"
                             "Content-Type: text/html\r\n\r\n"
            );

            strcat(response, content);
        } else {

            strcat(response, "HTTP/1.1 404 Not Found "
                             "Content-Encoding: gzip\r\n"
                             "Content-Language: en\r\n"
                             "Content-Type: text/html\r\n\r\n"
                             "<h1>Error 404 File not found!</h1>");
        }
    }
    int res = send(client,response,strlen(response),0);
    if(res == SOCKET_ERROR){
        logger_log("Sending failed!");
        return -1;
    }
    shutdown(client,SD_SEND);
    closesocket(client);
}

int webio_handlePOSTrequest(SOCKET client,WebIO wio,Peer list[],int peerCount){

}
char* webio_getHeader(char* folder) {
    FILE *fp;
    char *path = (char*) calloc(65,1);
    strcat(path, folder);
    strcat(path, "/header.html");

    fp = fopen(path, "r");


    if (fp != NULL) {
        char *content = (char *) calloc(sizeof(char) * 8192,1);
        char buf[1024];
        while (fgets(buf, 1024, fp) != NULL) {
            strcat(content, buf);
           // printf("%s",buf);
        }
        fclose(fp);
        return content;
    }else return "<html>";
}

char* getIndex(char* folder,Peer list[],int count){
    char* content = (char*) calloc(sizeof(char*)*8192,1);
    char * header = webio_getHeader(folder);

    strcat(content,header);
    strcat(content,"<h1>Peers:</h1>\n");
    if(count > 0) {
        strcat(content, "<ul>\n");
        for (int i = 0; i < count; ++i) {
            strcat(content, "<li>");
            strcat(content, list[i].peerData.id);
            strcat(content, "</li>\n");
        }
        strcat(content, "</ul>\n");
    }else
        strcat(content,"<div class=\"alert alert-warning\" role=\"alert\">\n"
                       "  No peers connected!\n"
                       "</div>\n");
    strcat(content,"<script>setTimeout(function(){\n"
                   "   window.location.reload(1);\n"
                   "}, 5000);</script>\n");

    strcat(content,"</div>\n");
    strcat(content,"</body>\n");
    strcat(content,"</html>\n");
    return content;
}
