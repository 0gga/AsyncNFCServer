#pragma once
#include "TcpServer.h"

#include <unordered_map>
#include <fstream>

#include "json.hpp"

enum class NFCState {
    idle,
    cliActive,
    clientActive
};

class Reader {
public:
    Reader(int, int);
    ~Reader();

    NFCState getState() const;
    void tick();

private:
    NFCState state = NFCState::idle;

    void handleState();

    void handleClient();
    void handleCli();

    void handleIdle();
    void handleNewUser();

    nlohmann::json getLog() const;

    TcpServer<std::string> clientTcp;
    TcpServer<std::string> cliTcp;

    nlohmann::json log;
    std::unordered_map<std::string, std::pair<std::string, int>> users;

    int readerAccessLevel;
};
