#pragma once
#define BOOST_ERROR_CODE_HEADER_ONLY

#include <string>
#include <vector>

#include <thread>

#include "json.hpp"

template<typename Rx>
class TcpServer {
public:
    TcpServer();
    ~TcpServer();

    void startAccept();

    /////////////

    void initClient(uint16_t);

    void send(nlohmann::json) const;
    void send(std::string) const;

    Rx*& getPackage() const;

private:
    static std::thread asyncTcp_t;

    Rx* package = new Rx;

    static inline bool running{true};
    static boost::asio::io_context io_context_;
};
