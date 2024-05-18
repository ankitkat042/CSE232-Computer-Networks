#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8081


void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
    }
}

int main() {
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int serverfd, clientfd;
    signal(SIGCHLD, sigchld_handler);

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) {
        perror("Cannot create socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if (bind(serverfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverfd);
        exit(1);
    }

    if (listen(serverfd, SOMAXCONN) < 0) {
        perror("Listen failed");
        close(serverfd);
        exit(1);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
    clientfd = accept(serverfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientfd < 0) {
        perror("Accept failed");
        continue;
    }
    printf("Connected to Client\n");

    if (fork() == 0) {
        close(serverfd);

        uint64_t num;
        while (1) {
            ssize_t bytes_read = read(clientfd, &num, sizeof(num));
            if (bytes_read <= 0) {
                break;
            }

            uint64_t n = ntohl(num);

            if (n == 0) {
                n = htonl(1);
                write(clientfd, &n, sizeof(n));
                continue;
            }
            if (n > 20) {
                n = 20;
            }
            long long int fact = 1;
            for (int i = 2; i <= n; i++) {
                fact *= i;
            }

            uint64_t result = n;
            
            result = htonl(result);
            write(clientfd, &result, sizeof(result));
        }
        printf("\n");

        close(clientfd);
        exit(0);
    } else { 
        close(clientfd);
    }
}
    close(serverfd);
    return 0;
}
