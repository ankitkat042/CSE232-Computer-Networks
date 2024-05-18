#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#define PORT 8081

void *single_client(void *arg) {
    if(arg == NULL){
        perror("cannot malloc()");
        return NULL;
    }

    int clientfd = *((int *)arg);
    if(arg == NULL){
        perror("cannot malloc()");
        return NULL;
    }
    free(arg);

    char buffer[1024];
    int retnum;

    while (1) {
        retnum = recv(clientfd, buffer, 1024, 0);
        if (retnum == 0) {
            break; 
        } else if (retnum == -1) {
            perror("cannot recv()");
            break; 
        } else {
            int n = atoi(buffer);
            // printf("Received from client %d\n", n);
            if (n > 20) {
                n = 20;
            }
            long long int fact = 1;
            for (int i = 2; i <= n; i++) {
                fact *= i;
            }
            snprintf(buffer, 1024, "%llu", fact);
            retnum = send(clientfd, buffer, strlen(buffer), 0);
            // printf("Sending to client%lld\n", fact);
            if (retnum == -1) {
                perror("cannot send()");
                break; 
            }
        }
    }
    close(clientfd);
    return NULL;
}

int main() {
    int serverfd, clientfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t clientaddr_sz = sizeof(clientaddr);
    pthread_t tid;


    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);

    if (bind(serverfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind() error");
        exit(1);
    }

    if (listen(serverfd, SOMAXCONN) == -1) {
        perror("listen() error");
        exit(1);
    }
    
    printf("Server is listening on port %d\n", PORT);

    while (1) {
        if ((clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &clientaddr_sz)) == -1) {
            perror("accept() error");
            continue;
        }
        printf("Connected to client [%d]\n", clientfd);

        int *clientfd_ptr = (int *)malloc(sizeof(int));
        if(clientfd_ptr==NULL){
            perror("cannot malloc()");
            close(clientfd);
            continue;
        }
        *clientfd_ptr = clientfd;

        if (pthread_create(&tid, NULL, single_client, (void *)clientfd_ptr) != 0){
            if (clientfd_ptr == NULL){
                perror("cannot malloc()");
                close(clientfd);
                continue;
            }
            free(clientfd_ptr);
            close(clientfd);
            perror("cannnot pthread_create()");
            continue;
        }
    }

    close(serverfd);
    return 0;
}

