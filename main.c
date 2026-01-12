#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>


void * handleClient(void * arg) {
    int client_fd = * (int *) arg;
    free(arg);

    char buffer[1024] = {0};
    read(client_fd, buffer, 1024);

    char requestType[1024], filePath[1024], protocolType[1024];
    int parsed = sscanf(buffer, "%s %s %s", requestType, filePath, protocolType);

    if (parsed < 2) {
        close(client_fd);
        return NULL;
    }

    // final path
    char filePathWithFolder[1100];
    strcpy(filePathWithFolder, "files");

    if (strcmp(filePath, "/") == 0) {
        strcat(filePathWithFolder, "/index.html");
    } else {
        strcat(filePathWithFolder, filePath);
    }

    FILE * file = fopen(filePathWithFolder, "r");
    int statusCode = 200;

    if (!file) {
        statusCode = 404;
        file = fopen("files/404.html", "r"); // Relative path!
    }

    char contentType[32] = "text/html";
    if (strstr(filePathWithFolder, ".ico")) strcpy(contentType, "image/x-icon");
    else if (strstr(filePathWithFolder, ".css")) strcpy(contentType, "text/css");
    else if (strstr(filePathWithFolder, ".png")) strcpy(contentType, "image/png");

    char header[200];
    sprintf(header, "HTTP/1.1 %s\r\nContent-Type: %s\r\n\r\n",
            (statusCode == 200) ? "200 OK" : "404 Not Found",
            contentType);
    write(client_fd, header, strlen(header));

    // send the content
    if (file) {
        char * content = (char *) malloc(1000);
        size_t contentCnt = 0;
        size_t contentCap = 1000;
        char c;
        while (fread(&c, 1, 1, file) == 1) {
            if (contentCnt + 1 >= contentCap) {
                contentCap *= 2;
                content = (char *) realloc(content, contentCap);
            }
            content[contentCnt++] = c;
        }
        write(client_fd, content, contentCnt);
        free(content);
        fclose(file);
    } else {
        char backup[50] = "<h1>404 Not Found</h1>";
        write(client_fd, backup, strlen(backup));
    }

    close(client_fd);
    return NULL;
}


int main() {
    // setup
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // bind and listen for requests
    if (server_fd < 0) { perror("Socket failed"); exit(1); }
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed"); exit(1);
    }
    listen(server_fd, 10);

    printf("Server listening on http://localhost:8080\n");

    // main loop
    while(1) {
        int client_fd = accept(server_fd, NULL, NULL);

        // handle request on a new thread
        pthread_t thread_id;
        int * pclient = (int *) malloc(sizeof(int));
        *pclient = client_fd;

        // start thread
        pthread_create(&thread_id, NULL, handleClient, pclient);

        // When done end thread
        pthread_detach(thread_id);
    }
}
