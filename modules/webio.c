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


int webio_create(int port,char* folder,struct Node_data myData,bool wildcard,WebIO *webIo){

    struct addrinfo *result = NULL;
    SOCKET listening;
    result = tcp_createIPv4Socket(&listening,port,wildcard);
    if(result == NULL){
        return 1;
    }
    tcp_bindnlisten(listening,result,SOMAXCONN);
    WebIO wio;

    wio.sockaddr = tcp_getAddr_in(listening);
    wio.socket= listening;
    wio.nodeData = myData;
    strcpy(wio.folder,folder);
    *webIo = wio;
    return 0;
}

int webio_handleRequest(WebIO wio,peerList list){
    SOCKET client = accept(wio.socket,NULL,NULL);
    char buf[8192];
    ZeroMemory(buf,8192);
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
        res = webio_handleGETrequest(client,wio,file, list);
    }else if(strcmp(req,"POST") == 0) {
        int i = strlen(buf)-1;
        while(buf[i] != '\n'){
            i--;
        }
        i++;
        char tmp[8196];
        strcpy(tmp,buf+i);
        map post = getHandshakeData(tmp);

        res = webio_handlePOSTrequest(client, wio,list,post);
        free(post.pairs);
    }else
        res = -1;

    return res;

}

char* webio_getMIMEtype(char* filename){
    char* ext = webio_getFiletype(filename);
    if (strcmp(ext,"html") == 0)
        return "text/html";
    else if (strcmp(ext, "json") == 0)
        return "application/json";
    else if (strcmp(ext, "svg") == 0)
        return "image/svg+xml";
    else if (strcmp(ext, "js")== 0)
        return "application/javascript";
    else if (strcmp(ext, "css")== 0)
        return "text/css";
    else if (strcmp(ext, "jpg")== 0)
        return "image/jpeg";
    else if (strcmp(ext, "png")== 0)
        return "image/png";
    else if (strcmp(ext, "ico")== 0)
        return "image/x-icon";
    else if (strcmp(ext, "xml")== 0)
        return "text/xml";
    else if (strcmp(ext, "zip")== 0)
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

int webio_handleGETrequest(SOCKET client,WebIO wio,char* file,peerList list){

    char buf[8192];
    sscanf(buf,"%*s %s",file);

    char path[129];

    ZeroMemory(path,sizeof(path));
    strcat(path,wio.folder);
    char *response = (char *) malloc(sizeof(char) * 8192);
    if (file[0] == '/')
        memmove(file, file+1, strlen(file));
    if(strlen(file) == 0) {
        char index[8192];
        getIndex(wio.folder, list, index);
        sprintf(response, "HTTP/1.1 200 OK "
                          "Content-Encoding: gzip\r\n"
                          "Content-Language: en\r\n"
                          "Content-Type: text/html\r\n\r\n%s",index);

    }else if(peer_isFoundInList(list,file)){
        sprintf(response, "HTTP/1.1 200 OK "
                          "Content-Encoding: gzip\r\n"
                          "Content-Language: en\r\n"
                          "Content-Type: text/html\r\n\r\n%s"
                ,getPeerPage(wio.folder,peer_getPeerByID(list,file) ) );
    }else if(strcmp(file,"kill") == 0){
        logger_log("Exit sign received.Exiting...");
        return -2;
    } else{
        strcat(path, file);
        FILE *fp;
        fp = fopen(path, "r");

        if (fp != NULL) {
            char *content = (char *) calloc(sizeof(char) * 16384, sizeof(char));

            char buf[4096];
            int counter = 0;

            while (fgets(buf, 4096, fp) != NULL) {
                int len = strlen(content);
                strcat(content, buf);
                counter += 4096;
                if (counter >= len) {
                    content = realloc(content, counter + 16384);
                }
            }

            fclose(fp);
            int len = counter + 1024;
            response = (char *) realloc(response,sizeof(char) * len);

            sprintf(response, "HTTP/1.1 200 OK "
                              "Content-Encoding: gzip\r\n"
                              "Content-Language: en\r\n"
                              "Content-Type: %s\r\n\r\n%s"
                    ,webio_getMIMEtype(file), content);

            free(content);
        } else {

            sprintf(response, "HTTP/1.1 404 Not Found "
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
    shutdown(client,SD_BOTH);
    closesocket(client);

    free(response);
}

int webio_handlePOSTrequest(SOCKET client,WebIO wio,peerList list,map post){
    shutdown(client,SD_RECEIVE);
    char *response = (char *) malloc(sizeof(char) * 8192);
    sprintf(response, "HTTP/1.1 304 Not Modified "

    );

    int res = send(client,response,strlen(response),0);
    if(res == SOCKET_ERROR ) {
        logger_log("Error with io");
        return -1;
    }
    shutdown(client,SD_SEND);

    if(map_isFound(post,"id") && map_isFound(post,"message") && strcmp(map_getValue(post,"message"),"%0D%0A") != 0){
        char file[64];
        sprintf(file,"%s%s.txt",wio.folder,map_getValue(post,"id"));
        FILE * f;
        f = fopen(file,"a");
        fprintf(f,"Me: %s\n",map_getValue(post,"message"));
        fclose(f);
        char buf[DEFAULT_BUFLEN];
        sprintf(buf,"@message=%s",map_getValue(post,"message"));
        res = send(list.array[peer_getIDPeer(list,map_getValue(post,"id"))].socket,buf,DEFAULT_BUFLEN,0);
        if(res == SOCKET_ERROR){
            logger_log("Error sending message.Error: %d",WSAGetLastError());
            return -1;
        }
        logger_log("Message sent to %s",map_getValue(post,"id"));
    }else map_dump(post);
    //free(response);
}
void webio_getHeader(char* folder,char**result) {

    char path[65];
    strcpy(path, folder);
    strcat(path, "/header.html");

    FILE* fp;
    fp = fopen(path, "r");

    if (fp != NULL) {
        *result = (char *) calloc(sizeof(char) * 8192,1);
        char buf[1024];
        while (fgets(buf, 1024, fp) != NULL) {
            strcat(*result, buf);
        }
        fclose(fp);
    }else
        *result = "<html>";
}

void getIndex(char* folder,peerList list,char outputBuffer[]){
    char content[8192];
    char* header;
    webio_getHeader(folder, &header);
    strcpy(content,header);
    free(header);
    strcat(content,"<h1>Peers:</h1>\n");
    if(list.length > 0) {
        strcat(content, "<ul>\n");
        for (int i = 0; i < list.length; ++i) {
            sprintf(content, "%s<li>"
                             "<a href=\"%s\">",
            content, list.array[i].peerData.id);
            if(strlen(list.array[i].peerData.nick) != 0){
                sprintf(content, "%s%s - ",content,list.array[i].peerData.nick);
            }
            sprintf(content,"%s%s</a></li>\n",content, list.array[i].peerData.id);
        }
        strcat(content, "</ul>\n");
    }else
        sprintf(content,"%s<div class=\"alert alert-warning\" role=\"alert\">\n"
                       "  No peers connected!\n"
                       "</div>\n",content);
    sprintf(content,"%s<script>setTimeout(function(){\n"
                   "   window.location.reload(1);\n"
                   "}, 5000);</script>\n"
                   "</div>\n"
                   "</body>\n"
    "</html>\n",content);
    strcpy(outputBuffer,content);
}

char *getPeerPage(char *folder, Peer p) {
    char* content = (char*) calloc(sizeof(char*)*8192,1);
    char* header;
    webio_getHeader(folder, &header);

    sprintf(content,"%s\n"
                    "<h1>%s</h1>\n"
                    "<div id=\"msgs\" style=\"margin-bottom:5em\"></div>\n"
                    "<div id=\"end\"></div>\n"
                    "    <form name=\"sendmsg\" class=\"form-inline\" style=\"margin: 7px;padding: 7px;position: fixed;bottom: 0;width: 100%%;\">"
                    "<textarea name=\"message\" id=\"message\" class=\"form-control\" style=\"width: 90%%;display: block;\"></textarea>"
                    "<input name=\"id\"class=\"form-control\" type=\"text\" style=\"display:none;\" value=\"%s\">"
                    "<input onclick=\"submitForm();\"name=\"s\" id=\"s\" type=\"button\" class=\"btn btn-primary\" style=\"margin: 7px;padding: 7px;display:inline;\" value=\"Send\" /></form>\n"
                    "<script src=\"assets/js/chat.js\"></script>"
                    "</body>\n"
                    "\n"
                    "</html>",header,p.peerData.id,p.peerData.id,p.peerData.id);
    free(header);
    return content;
}
