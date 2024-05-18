#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <poll.h>

#define PORT 8081
#define LIMIT 10240


int main() {
    int serverfd, clientfds[LIMIT] = {0};
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        fprintf(stderr, "Could not create a socket\n");
        exit(1);
    }
    
    int socketOption = 1;
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (char*)&socketOption, sizeof(socketOption));

    struct sockaddr_in serverfdAddr;
    serverfdAddr.sin_family = AF_INET;
    serverfdAddr.sin_port = htons(PORT);
    serverfdAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverfd, (struct sockaddr *)&serverfdAddr, sizeof(serverfdAddr)) == -1) {
        fprintf(stderr, "Could not bind to port %d\n", PORT);
        exit(1);
    }

    if (listen(serverfd, LIMIT) == -1) {
        fprintf(stderr, "Could not listen");
        exit(1);
    }
    printf("Server listening to port %d\n", PORT);

    struct pollfd sock_fds[LIMIT + 1] = {0};
    sock_fds[0].fd = serverfd;
    sock_fds[0].events = POLLIN;

    for (;;) {
        int activity = poll(sock_fds, LIMIT, -1);
        if (activity < 0) {
            fprintf(stderr, "Poll resulted in failure\n");
            continue;
        }
        if (sock_fds[0].revents & POLLIN) {
            socklen_t addrSize = sizeof(serverfdAddr);
            int clientfd = accept(serverfd, (struct sockaddr *)&serverfdAddr, &addrSize);
            if (clientfd == -1) {
                fprintf(stderr, "Could not connect to client\n");
                continue;
            }
            printf("Connected to client [%d]\n", clientfd);

            for (int i = 1; i <= LIMIT; ++i) {
                if (clientfds[i - 1] == 0) {
                    clientfds[i - 1] = clientfd;
                    sock_fds[i].fd = clientfd;
                    sock_fds[i].events = POLLIN;
                    break;
                }
                if (i == LIMIT) {
                    fprintf(stderr, "Max clients reached\n");
                    exit(1);
                }                    
            }
        }

        for (int i = 1; i <= LIMIT; ++i) {
            if (clientfds[i-1] != 0 && sock_fds[i].revents & POLLIN) {
                char buff[1024];
                int bytes_received;
                while (bytes_received = recv(clientfds[i-1], buff, sizeof(buff), 0) > 0) {
                    int num = atoi(buff);
                    if (num > 20) num = 20;
                    uint64_t fact = 1;
                    for (int i = 2; i <= num; ++i) fact *= i;
                    snprintf(buff, sizeof(buff), "%"PRIu64, fact);
                    send(clientfds[i-1], buff, strlen(buff), 0);
                }
                sock_fds[i].fd = -1;
                clientfds[i - 1] = 0;
            }
        }
    }
    
    close(serverfd);
    return 0;
}