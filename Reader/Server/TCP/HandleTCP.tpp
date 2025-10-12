#include "HandleTCP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE_RX 256
#define BUFSIZE_TX 512

template<typename Rx>
HandleTCP<Rx>::HandleTCP() {

}

template<typename Rx>
HandleTCP<Rx>::~HandleTCP() {
    running = false;
    if (Tcp_t.joinable()) {
        Tcp_t.join();
    }
    delete package;
}

template<typename Rx>
void HandleTCP<Rx>::initClient(uint16_t port) {}

template<typename Rx>
void HandleTCP<Rx>::initCli(uint16_t port) {
    printf("Starting CLI server...\n");

    int sockfd, newsockfd;

    socklen_t clilen;
    uint8_t bufferRx[BUFSIZE_RX];
    uint8_t bufferTx[BUFSIZE_TX];
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening CLI socket");
        return;
    }

    ////////////////////////////////////////

    constexpr char ip[]     = "172.16.15.2";

    ////////////////////////////////////////

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("sendto");
        close(sockfd);
        return;
    }

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
    }

    // Waiting for clients - Size of queue.
    listen(sockfd, 5);

    printf("CLI Listening...\n");
    clilen = sizeof(serv_addr);

    while (running) {
        printf("CLI Accepting...\n");
        newsockfd = accept(sockfd, (struct sockaddr*)&serv_addr, &clilen);

        if (newsockfd < 0) perror("ERROR on accept");

        printf("CLI Accepted\n");

        readTextTCP(newsockfd, bufferRx, sizeof(bufferRx));
        n = read(newsockfd, bufferRx, sizeof(bufferRx));

        if (n < 0) perror("ERROR reading from socket");
        printf("CLI Message: %s\n", (char*)bufferRx);

        snprintf((char*)bufferTx, sizeof(bufferTx), "CLI Got message: %s", (char*)bufferRx);

        n = write(newsockfd, bufferTx, strlen((char*)bufferTx));
        if (n < 0) perror("ERROR writing to socket");

        close(newsockfd);
    }
    close(sockfd);
}

template<typename Rx>
void HandleTCP<Rx>::send(nlohmann::json data) const {}

template<typename Rx>
void HandleTCP<Rx>::send(std::string msg) const {}

template<typename Rx>
Rx*& HandleTCP<Rx>::getPackage() const {
    return package;
}
