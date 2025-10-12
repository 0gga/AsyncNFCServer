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

    void initClient();
    void initCli();

    void send(nlohmann::json) const;
    void send(std::string) const;

    Rx*& getPackage() const;

private:
    std::thread clientTcp_t;
    std::thread cliTcp_t;

    Rx* package = new Rx;
    bool running{true};
};
