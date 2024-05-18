#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <sys/epoll.h>

#define PORT 8081
#define MAX_CLIENTS 10240

int main() {
    int serverfd, clientfds[MAX_CLIENTS] = {0};
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

    if (bind(serverfd, (struct sockaddr*)&serverfdAddr, sizeof(serverfdAddr)) == -1) {
        fprintf(stderr, "Could not bind server");
        exit(1);
    }

    if (listen(serverfd, MAX_CLIENTS) == -1) {
        fprintf(stderr, "Cannot listen to server");
        exit(1);
    }
    
    printf("Server listening to port %d\n", PORT);
    

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        fprintf(stderr, "Cannot create epoll instance\n");
        exit(1);
    }

    struct epoll_event event, events[MAX_CLIENTS + 1];
    event.events = EPOLLIN;
    event.data.fd = serverfd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverfd, &event) == -1) {
        fprintf(stderr, "Cannot add server to epoll\n");
        exit(1);
    }

    for (;;) {
        int eventNo = epoll_wait(epoll_fd, events, MAX_CLIENTS + 1, -1);
        if (eventNo < 0) {
            fprintf(stderr, "Epoll wait error\n");
            exit(1);
        }

        for (int i = 0; i < eventNo; ++i) {
            if (events[i].data.fd == serverfd) {
                socklen_t addrSize = sizeof(serverfdAddr);
                int clientfd = accept(serverfd, (struct sockaddr*)&serverfdAddr, &addrSize);
                if (clientfd == -1) {
                    continue;
                    fprintf(stderr, "Could not accept the client connection\n");
                }   
                printf("Connected to client [%d]\n", clientfd);

                for (int j = 0; j < MAX_CLIENTS; ++j) {
                    if (clientfds[j] == 0) {
                        clientfds[j] = clientfd;
                        event.events = EPOLLIN | EPOLLET;
                        event.data.fd = clientfd;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientfd, &event) == -1) {
                            fprintf(stderr, "Cannot add client to epoll\n");
                            exit(1);
                        }
                        break;
                    }
                    if (j == MAX_CLIENTS) 
                        fprintf(stderr, "Max clients reached");
                }
            } else {
                int clientfd = events[i].data.fd;
                char What[1024];
                int bytes_received;
                while (bytes_received = recv(clientfd, What, sizeof(What), 0) > 0) {
                    int num = atoi(What);
                    if (num > 20) num = 20;
                    uint64_t fact = 1;
                    for (int i = 2; i <= num; i++) fact *= i;
                    snprintf(What, 1024, "%" PRIu64, fact);
                    send(clientfd, What, strlen(What), 0);
                }
                                
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientfd, NULL) == -1) {
                    fprintf(stderr, "Cannot remove client from epoll\n");
                    exit(1);
                }
                
                close(clientfd);
            }
        }
    }

    close(serverfd);
    return 0;
}