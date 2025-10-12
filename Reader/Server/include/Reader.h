#pragma once
#include "HandleTCP.h"

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
    Reader(int);
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

    HandleTCP<std::string> clientTcp;
    HandleTCP<std::string> cliTcp;

    nlohmann::json log;
    std::unordered_map<std::string, std::pair<std::string, int>> users;
};
