#include "HandleTCP.h"


#define BUFSIZE_RX 256
#define BUFSIZE_TX 512

HandleTCP::HandleTCP() {
    //client_TCP_t = std::thread(&initClient);
    CLI_TCP_t = std::thread(&initCLI);
}

HandleTCP::~HandleTCP() {
    if (client_TCP_t.joinable()) {
        client_TCP_t.join();
    }
    if (CLI_TCP_t.joinable()) {
        CLI_TCP_t.join();
    }
    delete client_package;
    delete CLI_package;
}

void HandleTCP::initClient() {

}

void HandleTCP::initCLI() {
    bool running{true};

    printf("Starting server...\n");

    int sockfd, newsockfd;

    socklen_t clilen;
    uint8_t bufferRx[BUFSIZE_RX];
    uint8_t bufferTx[BUFSIZE_TX];
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return;
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
        return;
    }

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
    }

    // Waiting for clients - Size of queue.
    listen(sockfd, 5);

    printf("Listening...\n");
    clilen = sizeof(serv_addr);

    while (running) {
        printf("Accepting...\n");
        newsockfd = accept(sockfd, (struct sockaddr*)&serv_addr, &clilen);

        if (newsockfd < 0)
            perror("ERROR on accept");

        printf("Accepted\n");

        readTextTCP(newsockfd, bufferRx, sizeof(bufferRx));
        n = read(newsockfd, bufferRx, sizeof(bufferRx));

        if (n < 0)
            perror("ERROR reading from socket");
        printf("Message: %s\n", (char*)bufferRx);

        snprintf((char*)bufferTx, sizeof(bufferTx), "Got message: %s", (char*)bufferRx);

        n = write(newsockfd, bufferTx, strlen((char*)bufferTx));
        if (n < 0)
            perror("ERROR writing to socket");

        close(newsockfd);
    }
    close(sockfd);
}

std::pair<int, int>* HandleTCP::getClientPackage() const {
    return client_package;
}

std::string* HandleTCP::getCLIPackage() const {
    return CLI_package;
}
