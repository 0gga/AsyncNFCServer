#pragma once
#include <string>
#include <vector>

#include <thread>

template<typename Rx, typename Tx>
class HandleTCP {
public:
    HandleTCP();
    ~HandleTCP();

    void initClient();
    void initCLI();

    void send(Tx) const;

    Rx getPackage() const;

private:
    std::thread client_TCP_t;
    std::thread CLI_TCP_t;

    Rx* package = new Rx;
};
