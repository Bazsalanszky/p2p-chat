//
// Készítette: Toldi Balázs Ádám
// Dátum:  2019. 10. 14..
//
#include "webio.h"

int webio_create(Config config, struct PeerList *list, WebIO *webIo) {
    char *port = map_getValue(config, "interface-port");
    if (port == NULL)
        port = DEFAULT_INTERFACE_PORT;
    char *folder = map_getValue(config, "interface-folder");
    if (folder == NULL)
        folder = DEFAULT_WWW_FOLDER;
    char *local_mode_str = map_getValue(config, "interface-local");
    bool local_mode = false;
    if (local_mode_str != NULL && strcmp(local_mode_str, "true") == 0)
        local_mode = true;

    struct addrinfo *result = NULL;
    SOCKET listening;
    result = tcp_createIPv4Socket(&listening, atoi(port), !local_mode);
    if (result == NULL) {
        return 3;
    }
    int res = tcp_bindnlisten(listening, result, SOMAXCONN);
    if (res != 0) {
        return res;
    }
    WebIO wio;

    wio.socket = listening;
    wio.list = list;
    strcpy(wio.folder, folder);
    *webIo = wio;
    return 0;
}

int webio_handleRequest(WebIO wio) {
    SOCKET client = accept(wio.socket, NULL, NULL);
    char buf[8192];
    memset(buf, 0, 8192);
    int res = recv(client, buf, 8192, 0);
    if (res <= 0) {
        logger_log("Error with web interface!");
        closesocket(client);
        return -1;
    }
    char method[5],v_major[2],v_minor[3],file[50];
    sscanf(buf, "%s %s %*[^/]/%[^.].%s",method,file,v_major,v_minor);
    if(strcmp(v_major,"1") != 0){
        char *response = "HTTP/1.0 505 HTTP Version Not Supportedd"
                         "Content-Encoding: gzip\r\n"
                         "Content-Language: en\r\n"
                         "Content-Type: text/html\r\n\r\n"
                         "<h1>Error 505: HTTP version not supportedd!</h1>";
        res = send(client, response, (int) strlen(response), 0);
        if (res == SOCKET_ERROR) {
            logger_log("Error sending 505 page!");
        }
        return -1;
    }
    if (strcmp(method, "GET") == 0) {
        res = webio_handleGETrequest(client, wio, file);

    } else if (strcmp(method, "POST") == 0) {
        int i = (int) strlen(buf) - 1;
        while (buf[i] != '\n') {
            i--;
        }
        i++;
        char tmp[8196];
        strcpy(tmp, buf + i);
        Map post = getPacketData(tmp);

        res = webio_handlePOSTrequest(client, wio, post);
        free(post.pairs);
    } else{
        char *response = "HTTP/1.0 501 Not Implemented"
                         "Content-Encoding: gzip\r\n"
                         "Content-Language: en\r\n"
                         "Content-Type: text/html\r\n\r\n"
                         "<h1>Error 501: Method not implemented!</h1>";
        res = send(client, response, (int) strlen(response), 0);
        if (res == SOCKET_ERROR) {
            logger_log("Error sending 501 page!");
        }
    }


    return res;
}

char *webio_getMIMEtype(char *filename) {
    char type[10];
    char *ext = webio_getFiletype(filename);
    strcpy(type, ext);
    free(ext);
    if (strcmp(type, "html") == 0)
        return "text/html";
    else if (strcmp(type, "json") == 0)
        return "application/json";
    else if (strcmp(type, "svg") == 0)
        return "image/svg+xml";
    else if (strcmp(type, "js") == 0)
        return "application/javascript";
    else if (strcmp(type, "css") == 0)
        return "text/css";
    else if (strcmp(type, "jpg") == 0)
        return "image/jpeg";
    else if (strcmp(type, "png") == 0)
        return "image/png";
    else if (strcmp(type, "bmp") == 0)
        return "image/bmp";
    else if (strcmp(type, "ico") == 0)
        return "image/x-icon";
    else if (strcmp(type, "xml") == 0)
        return "text/xml";
    else if (strcmp(type, "zip") == 0)
        return "application/zip";
    else return "text/plain";
}

char *webio_getFiletype(char *filename) {
    char *ext = (char *) malloc(sizeof(char) * 10);
    strcpy(ext,"");
    char *tmp = strtok(filename, ".");
    while (tmp != NULL) {
        strncpy(ext, tmp, 10);
        tmp = strtok(NULL, ".");
    }
    return ext;
}

static int webio_handleGETrequest(SOCKET client, WebIO wio, char *file) {

    char path[129];

    memset(path, 0, sizeof(path));
    strcat(path, wio.folder);
    int len = 0;
    if (file[0] == '/')
        memmove(file, file + 1, strlen(file));
    if (strcmp(file, "") == 0) {
        char index[8192] = "";
        webio_getIndex(wio, index);
        webio_sendPage(client, index);
    } else if (peer_ID_isFound(*(wio.list), file) || webio_isPeerFound(wio.folder, file)) {
        char cnt[8192] = "";
        webio_getPeerPage(wio, file, cnt);
        webio_sendPage(client, cnt);
    } else if (strcmp(file, "kill") == 0) {
        webio_sendPage(client, "<h1>Server Killed!</h1>");
        logger_log("Exit sign received.Exiting...");
        return -2;
    } else {
        strcat(path, file);
        //File küldés windows-on
#ifdef _WIN32
        HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
        DWORD size = GetFileSize(file, NULL);
        if (file == INVALID_HANDLE_VALUE || size < 0)
            webio_send404Page(client);
        else {
            webio_sendOKHeader_wSize(client, path, size);
            TransmitFile(client, file, 0, 0, NULL, NULL, 0);
            CloseHandle(file);
        }
#else
        int fd = open(path, O_RDONLY);
        if (fd == -1)
            webio_send404Page(client);
        else {
            struct stat stat_struct;
            fstat(fd, &stat_struct);
            int length = stat_struct.st_size;
            webio_sendOKHeader(client, path);
            size_t total_bytes_sent = 0;
            ssize_t bytes_sent;
            char puf[len];
            while (total_bytes_sent < length) {
                bytes_sent = sendfile(client,fd,0,length);
                if(bytes_sent == -1) logger_log("Error: %s",strerror(errno));
                total_bytes_sent += bytes_sent;
            }
        }
#endif
        shutdown(client, SD_BOTH);
    }
    closesocket(client);
    return 0;
}

static int webio_handlePOSTrequest(SOCKET client, WebIO wio, Map post) {
    shutdown(client, SD_RECEIVE);
    char *response = "HTTP/1.0 304 Not Modified ";

    int res = send(client, response, (int) strlen(response), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error with io");
        return -1;
    }
    shutdown(client, SD_BOTH);

    if (map_isFound(post, "id") && map_isFound(post, "message") &&
        strcmp(map_getValue(post, "message"), "%0D%0A") != 0) {
        char file[64];
        char folder[72];
        sprintf(folder, "%s/peers/", wio.folder);
#if defined(_WIN32)
        CreateDirectoryA(folder,NULL);
#else
        mkdir(folder, 0777);
#endif
        sprintf(file, "%s%s.txt", folder, map_getValue(post, "id"));
        FILE *f;
        f = fopen(file, "a");
        fprintf(f, "Me: %s\n", map_getValue(post, "message"));
        fclose(f);
        char buf[DEFAULT_BUFLEN];
        int i = peer_ID_getPeer(*(wio.list), map_getValue(post, "id"));
        if (i < 0) {
            logger_log("Error sending message! Error: Peer offline!");
            return 1;
        }

        sprintf(buf, "@message=%s", map_getValue(post, "message"));
        res = send(wio.list->array[i].socket, buf, DEFAULT_BUFLEN, 0);
        if (res == SOCKET_ERROR) {
            logger_log("Error sending message.Error: %d", errno);
            return 2;
        }
        logger_log("Message sent to %s", map_getValue(post, "id"));
    }
    return 0;
}

static void webio_getHeader(char *folder, char result[]) {

    char path[65];
    strcpy(path, folder);
    strcat(path, "/header.html");

    FILE *fp;
    fp = fopen(path, "r");

    if (fp != NULL) {
        char buf[1024];
        while (fgets(buf, 1024, fp) != NULL) {
            strcat(result, buf);
        }
        fclose(fp);
    } else
        result = "<html>";
}

static void webio_getIndex(WebIO wio, char *outputBuffer) {
    char content[8192] = "";
    char header[4096] = "";

    webio_getHeader(wio.folder, header);
    strcpy(content, header);

    strcat(content, "<h1>Peers:</h1>\n");
    if (wio.list->length > 0) {
        strcat(content, "<ul>\n");
        for (size_t i = 0; i < wio.list->length; ++i) {
            sprintf(content, "%s<li>"
                             "<a href=\"%s\">",
                    content, wio.list->array[i].peerData.id);
            if (strcmp(wio.list->array[i].peerData.nick, "") != 0) {
                sprintf(content, "%s%s - ", content, wio.list->array[i].peerData.nick);
            }
            sprintf(content, "%s%s</a></li>\n", content, wio.list->array[i].peerData.id);
        }
        strcat(content, "</ul>\n");
    } else
        sprintf(content, "%s<div class=\"alert alert-warning\" role=\"alert\">\n"
                         "  No peers connected!\n"
                         "</div>\n", content);
    strcat(content, "<h1>Offline messages:</h1>\n");
    char path[65];
    sprintf(path, "%s/peers/", wio.folder);
#ifdef _MSC_VER
    HANDLE dir;
    WIN32_FIND_DATA file_data;
    strcat(path,"/*");
    int cnt = 0;
    if ((dir = FindFirstFile(path, &file_data)) != INVALID_HANDLE_VALUE)
    {
        strcat(content, "<ul>\n");
        do{
            if(strcmp(file_data.cFileName,".") == 0 || strcmp(file_data.cFileName,"..") == 0) continue;
            char peer[33];
            sscanf(file_data.cFileName,"%[^.]",peer);
            if(!peer_ID_isFound(*wio.list,peer)) {
                cnt++;
                sprintf(content, "%s<li><a href=\"%s\">%s</a></li>", content, peer, peer);
            }
        }while(FindNextFile(dir,&file_data));
        FindClose(dir);
        strcat(content, "</ul>\n");
    }
    if((dir = FindFirstFile(path, &file_data)) == INVALID_HANDLE_VALUE || cnt == 0){
        sprintf(content, "%s<div class=\"alert alert-warning\" role=\"alert\">\n"
                         "  No offline messages!\n"
                         "</div>\n", content);
    }
#else
    DIR *d;

    d = opendir(path);
    int cnt = 0;
    if (d != NULL){
        strcat(content, "<ul>\n");
        struct dirent *de;
        while ((de = readdir(d)) != NULL) {
            if(strcmp(de->d_name,".") == 0 || strcmp(de->d_name,"..") == 0) continue;
            char peer[33];
            sscanf(de->d_name,"%[^.]",peer);
            if(!peer_ID_isFound(*wio.list,peer)) {
                cnt++;
                sprintf(content, "%s<li><a href=\"%s\">%s</a></li>", content, peer, peer);
            }
        }
        closedir(d);
        strcat(content, "</ul>\n");
    }
    if(d == NULL || cnt ==0){
        sprintf(content, "%s<div class=\"alert alert-warning\" role=\"alert\">\n"
                         "  No offline messages!\n"
                         "</div>\n", content);
    }
#endif


    sprintf(content, "%s<script>setTimeout(function(){\n"
                     "   window.location.reload(1);\n"
                     "}, 5000);</script>\n"
                     "</div>\n"
                     "</body>\n"
                     "</html>\n", content);
    strcpy(outputBuffer, content);
}

static void webio_getPeerPage(WebIO wio, char *id, char *outputBuffer) {
    char content[8192] = "";
    char header[4096] = "";

    webio_getHeader(wio.folder, header);
    strcpy(content, header);
    bool online = peer_ID_isFound(*wio.list,id);
    char *img = (online) ? "<img width=\"30\" height=\"30\" src=\"assets\\img\\on.svg\">"
                         : "<img width=\"30\" height=\"30\" src=\"assets\\img\\off.svg\">";
    char *disabled = (online) ? "" : "disabled";
    char *nickname = (online) ? wio.list->array[peer_ID_getPeer(*wio.list,id)].peerData.nick:"";
    sprintf(content, "%s\n"
                     "<h1>%s%s %s</h1>\n"
                     "<div id=\"msgs\" style=\"margin-bottom:5em\"></div>\n"
                     "<div id=\"end\"></div>\n"
                     "    <form name=\"sendmsg\" class=\"form-inline\" style=\"margin: 7px;padding: 7px;position: fixed;bottom: 0;width: 100%%;\">"
                     "<textarea name=\"message\" id=\"message\" class=\"form-control\" style=\"width: 90%%;display: block;\" %s></textarea>"
                     "<input name=\"id\"class=\"form-control\" type=\"text\" style=\"display:none;\" value=\"%s\">"
                     "<input onclick=\"submitForm();\"name=\"s\" id=\"s\" type=\"button\" class=\"btn btn-primary\" style=\"margin: 7px;padding: 7px;display:inline;\" value=\"Send\" /></form>\n"
                     "<script src=\"assets/js/chat.js\"></script>"
                     "</body>\n"
                     "\n"
                     "</html>", header, img,nickname, id, disabled, id);
    strcpy(outputBuffer, content);
}

static bool webio_isPeerFound(char *folder, char *id) {

    char file[129];
    sprintf(file, "%s/peers/%s.txt", folder, id);
    FILE *F = fopen(file, "r");
    if (F == NULL) {
        return false;
    } else
        return true;
}

void webio_sendOKHeader(SOCKET socket, char *file) {
    char response[8192];
    sprintf(response, "HTTP/1.0 200 OK "
                      "Content-Encoding: gzip\r\n"
                      "Content-Language: en\r\n"
                      "Content-Type: %s\r\n\r\n", webio_getMIMEtype(file));
    int res = send(socket, response, (int) strlen(response), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending http ok header!");
    }
}

void webio_sendOKHeader_wSize(SOCKET socket, char *file, int size) {
    char response[8192];
    sprintf(response, "HTTP/1.0 200 OK "
                      "Content-Encoding: gzip\r\n"
                      "Content-Language: en\r\n"
                      "Content-Length: %d\r\n"
                      "Content-Type: %s\r\n\r\n", size, webio_getMIMEtype(file));
    int res = send(socket, response, (int) strlen(response), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending http ok header!");
    }
}

void webio_send404Page(SOCKET socket) {

    char *response = "HTTP/1.0 404 Not Found "
                     "Content-Encoding: gzip\r\n"
                     "Content-Language: en\r\n"
                     "Content-Type: text/html\r\n\r\n"
                     "<h1>Error 404 File not found!</h1>";
    int res = send(socket, response, (int) strlen(response), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending 404 page!");
    }
}

void webio_sendPage(SOCKET socket, char *content) {
    char f[] = "index.html";
    webio_sendOKHeader(socket, f);
    int res = send(socket, content, (int) strlen(content), 0);
    if (res == SOCKET_ERROR) {
        logger_log("Error sending page!");
    }
    shutdown(socket, SD_BOTH);
}