#pragma once
#include "HandleTCP.h"

#include <unordered_map>

#include "json.hpp"

enum class NFCState {
    idle,
    active,
    newUser
};

class State {
public:
    State();
    ~State();

    NFCState getState() const;
    void tick();

private:
    NFCState state = NFCState::idle;

    void handle_State();
    void handle_Client();
    void handle_CLI();

    void handle_Idle();
    void handle_Active();
    void handle_NewUser();

    nlohmann::json getLog() const;

    bool check_For_Tag() const;
    bool is_KnownTag() const;
    void process_KnownTag();
    void register_NewUser();

    HandleTCP<int, std::string> client_TCP;
    HandleTCP<std::string, nlohmann::json> CLI_TCP;

    nlohmann::json log;
};
