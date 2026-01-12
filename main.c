#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


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
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 10);

    printf("Server listening on http://localhost:8080\n");

    // main loop
    while(1) {
        int client_fd = accept(server_fd, NULL, NULL);

        // read the request
        char buffer[1024] = {0};
        read(client_fd, buffer, 1024);
        printf("%s\n", buffer);

        // send header
        char header[45] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        write(client_fd, header, strlen(header));

        // send content
        FILE * file = fopen("index.html", "r");

        if (file) {
            char * content = (char *) malloc(1000 * sizeof(char));
            size_t contentCnt = 0;
            size_t contentCap = 1000;

            char c;
            while (fscanf(file, "%c", &c) != EOF) {
                if (contentCnt + 1 >= contentCap) {
                    contentCap *= 2;
                    content = (char *) realloc(content, contentCap * sizeof(char));
                }

                content[contentCnt++] = c;
            }

            write(client_fd, content, contentCnt * sizeof(char));
            free(content);
            fclose(file);
        }

        close(client_fd);
    }
}
