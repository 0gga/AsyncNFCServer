#pragma once
#include <string>
#include <vector>

#include <thread>

#include "json.hpp"

template<typename Rx>
class TcpServer {
public:
    TcpServer();
    ~TcpServer();

    void initClient(uint16_t);
    void initCli(uint16_t);

    void send(nlohmann::json) const;
    void send(std::string) const;

    Rx*& getPackage() const;

private:
    std::thread asyncTcp_t;

    Rx* package = new Rx;

    bool running{true};
};
