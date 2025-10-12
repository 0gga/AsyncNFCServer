#pragma once
#include <string>
#include <vector>

#include <thread>

#include "json.hpp"

template<typename Rx>
class HandleTCP {
public:
    HandleTCP();
    ~HandleTCP();

    void initClient(uint16_t);
    void initCli(uint16_t);

    void send(nlohmann::json) const;
    void send(std::string) const;

    Rx*& getPackage() const;

private:
    std::thread Tcp_t;

    Rx* package = new Rx;

    bool running{true};
};
