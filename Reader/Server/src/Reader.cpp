#include "Reader.h"

NFCState Reader::getState() const {
    return state;
}

Reader::Reader(int port) {
    clientTcp.initClient(static_cast<uint16_t>(port));
    clientTcp.initCli(static_cast<uint16_t>(port));
}

Reader::~Reader() {}

void Reader::tick() {
    handleState();
}

void Reader::handleState() {
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

void Reader::handleClient() {
    auto& pkg = clientTcp.getPackage();
    // Check UID and access level.
    bool authorized = users.find
    if (authorized) {
        clientTcp.send(static_cast<std::string>("Approved"));
    } else
    {
        clientTcp.send(static_cast<std::string>("Denied"));
    }

    // Set package to nullptr for handleIdle
    pkg = nullptr;
    state = NFCState::idle;
}

void Reader::handleCli() {
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
    pkg = nullptr;
    state = NFCState::idle;
}

void Reader::handleIdle() {
    // Setting states if packages != nullptr.
    if (cliTcp.getPackage()) state = NFCState::cliActive;
    else if (clientTcp.getPackage()) state = NFCState::clientActive;
}

nlohmann::json Reader::getLog() const {
    return log;
}

void Reader::handleNewUser() {
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
