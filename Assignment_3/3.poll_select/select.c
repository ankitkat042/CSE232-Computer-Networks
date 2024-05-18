#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define PORT 8081
#define LIMIT 10240

int main() {
    int serverfd, clientfds[LIMIT] = {0};
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        fprintf(stderr, "Could not create a socket");
        exit(1);
    }
    
    int socketOption = 1;
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (char*)&socketOption, sizeof(socketOption));
 
    struct sockaddr_in serverfdAddr;
    serverfdAddr.sin_family = AF_INET;
    serverfdAddr.sin_port = htons(PORT);
    serverfdAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(serverfd, (struct sockaddr*)&serverfdAddr, sizeof(serverfdAddr)) == -1) {
        fprintf(stderr, "Could not bind to port");
        exit(1);
    }
    
    if (listen(serverfd, LIMIT) == -1) {
        fprintf(stderr, "Could not listen");
        exit(1);
    }
    printf("Server listening to port %d\n", PORT);
    
    fd_set read_fds;
    
    for (;;) {
        FD_ZERO(&read_fds);
        FD_SET(serverfd, &read_fds);
        int nfds = serverfd;
        
        for (int i = 0; i < LIMIT; ++i) {
            if (clientfds[i] > 0) {
                FD_SET(clientfds[i], &read_fds);
                if (clientfds[i] > nfds) nfds = clientfds[i];
            }
        }
        
        if (select(nfds+1, &read_fds, NULL, NULL, NULL) >= 0) {
            if (FD_ISSET(serverfd, &read_fds) != 0) {
                socklen_t addr_size = sizeof(serverfdAddr);
                int clientfd = accept(serverfd, (struct sockaddr*)&serverfdAddr, &addr_size);
                if (clientfd == -1) {
                    fprintf(stderr, "Could not accept the  connection");
                    continue;
                } 
                else printf("Connected to client [%d]\n", clientfd);
                
                for (int i = 0; i < LIMIT; ++i) {
                    if (clientfds[i] == 0) {
                        clientfds[i] = clientfd;
                        break;
                    }
                    if (i == LIMIT-1) 
                        fprintf(stderr, "Max clients reached cannot accept the connection\n");
                }
            }
            
            for (int i = 0; i < LIMIT; ++i) {
                if (clientfds[i] == 0) continue;
                if (FD_ISSET(clientfds[i], &read_fds)) {
                    char buff[1024];
                    int recvBytes;
                    while ((recvBytes = recv(clientfds[i], buff, sizeof(buff), 0)) > 0) {
                        int num = atoi(buff);
                        if (num > 20) num = 20;
                        uint64_t fact = 1;
                        for (int i = 2; i <= num; ++i) fact *= i;
                        snprintf(buff, 1024, "%"PRIu64, fact);
                        send(clientfds[i], buff, sizeof(buff), 0);
                    }
                    clientfds[i] = 0;
                }
            }
        }
    }
    
    close(serverfd);
    return 0;
}