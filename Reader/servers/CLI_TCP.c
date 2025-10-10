#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE_RX 200
#define BUFSIZE_TX 256

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {
    printf("Starting server...\n");

    int sockfd, newsockfd, portno;

    socklen_t clilen;
    uint8_t bufferRx[BUFSIZE_RX];
    uint8_t bufferTx[BUFSIZE_TX];
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return 1;
    }

    ////////////////////////////////////////

    constexpr char ip[]     = "172.16.15.2";
    constexpr uint16_t port = 9000;

    ////////////////////////////////////////

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("sendto");
        close(sockfd);
        return 1;
    }

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    // Waiting for clients - Size of queue.
    listen(sockfd, 5);

    printf("Listening...\n");
    clilen = sizeof(cli_addr);

    while (true) {
        printf("Accepting...\n");
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        if (newsockfd < 0) error("ERROR on accept");
        else printf("Accepted\n");

        readTextTCP(newsockfd, bufferRx, sizeof(bufferRx));
        n = read(newsockfd, bufferRx, sizeof(bufferRx));

        if (n < 0) error("ERROR reading from socket");
        printf("Message: %s\n", (char*)bufferRx);

        snprintf((char*)bufferTx, sizeof(bufferTx), "Got message: %s", (char*)bufferRx);

        n = write(newsockfd, bufferTx, strlen((char*)bufferTx));
        if (n < 0) error("ERROR writing to socket");

        close(newsockfd);
    }
    close(sockfd);
    return 0;
}