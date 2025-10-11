#pragma once
#include <string>
#include <vector>

#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class HandleTCP {
public:
    HandleTCP();
    ~HandleTCP();

    void initClient();
    void initCLI();

    std::pair<int, int>* getClientPackage() const;
    std::string* getCLIPackage() const;

private:
    std::thread client_TCP_t;
    std::thread CLI_TCP_t;

    std::pair<int, int>* client_package = new std::pair<int, int>;
    std::string* CLI_package = new std::string;
};
