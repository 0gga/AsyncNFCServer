#include "State.h"

NFCState State::getState() const {
    return state;
}

State::State() {
    client_TCP.initClient();
    client_TCP.initCLI();
}

State::~State() {}

void State::tick() {
    handle_CLI();
    handle_Client();
    handle_State();
}

void State::handle_State() {
    switch (state) {
        case NFCState::idle:
            handle_Idle();
            break;
        case NFCState::active:
            handle_Active();
            break;
        case NFCState::newUser:
            handle_NewUser();
            break;
    }
}

void State::handle_Client() {
    auto pkg = client_TCP.getClientPackage();
    if (!pkg) {
        return;
    }
}

void State::handle_CLI() {
    auto pkg = CLI_TCP.getCLIPackage();
    if (!pkg) {
        return;
    }
    if (*pkg == "newUser") {
        state = NFCState::newUser;
    }
    if (*pkg == "rmUser") {}
    if (*pkg == "getLog") {
        nlohmann::json log = getLog();
        CLI_TCP.send(log);
    }
}

void State::handle_Idle() {
    if (check_For_Tag()) state = NFCState::active;
}

void State::handle_Active() {
    if (!check_For_Tag()) state = NFCState::idle;
    else if (!is_KnownTag()) state = NFCState::newUser;
}

void State::handle_NewUser() {
    register_NewUser();
    state = NFCState::active;
}

nlohmann::json State::getLog() {

}
