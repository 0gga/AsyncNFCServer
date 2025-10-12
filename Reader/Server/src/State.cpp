#include "State.h"

NFCState State::getState() const {
    return state;
}

State::State() {
    clientTcp.initClient();
    clientTcp.initCli();
}

State::~State() {}

void State::tick() {
    handleState();
}

void State::handleState() {
    switch (state) {
        case NFCState::idle:
            handleIdle();
            break;
        case NFCState::cliActive:
            handleCli();
            break;
        case NFCState::clientActive:
            handleClient();
            break;
    }
}

void State::handleClient() {
    auto& pkg = clientTcp.getPackage();
    // Check UID and access level.
    if (1) {
        clientTcp.send(static_cast<std::string>("Approved"));
    } else
    {
        clientTcp.send(static_cast<std::string>("Denied"));
    }

    // Set package to nullptr for handleIdle
    pkg = nullptr;
}

void State::handleCli() {
    auto& pkg             = cliTcp.getPackage();
    std::string prefixStr = "newUser";
    if (pkg->substr(0, prefixStr.size()) == prefixStr) {
        pkg->erase(0, prefixStr.size());
        handleNewUser();
    }
    if (*pkg == "rmUser") {}
    if (*pkg == "getLog") {
        log = getLog();
        cliTcp.send(log);
    }
    delete pkg;
}

void State::handleIdle() {
    // Setting states if packages != nullptr.
    if (cliTcp.getPackage()) state = NFCState::cliActive;
    else if (clientTcp.getPackage()) state = NFCState::clientActive;
}

nlohmann::json State::getLog() const {
    return log;
}

void State::handleNewUser() {
    auto& clientPkg = clientTcp.getPackage();
    auto& cliPkg    = cliTcp.getPackage();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    if (!clientPkg) {
        return;
    }

    char accessLvl = cliPkg->front();
    cliPkg->erase(0, 1);
    std::string name = *cliPkg;

    nlohmann::json newUser{
        {"UID", *clientPkg},
        {"accessLvl", accessLvl},
        {"Name", *cliPkg}
    };
    users.push_back(newUser);
    clientPkg = nullptr;
    cliPkg    = nullptr;
    state     = NFCState::idle;
}
