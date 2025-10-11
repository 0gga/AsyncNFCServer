#pragma once
#include "HandleTCP.h"

#include <unordered_map>

enum class NFCState {
	idle,
	active,
	newUser
};

class State {
public:
	NFCState getState() const;
	void tick();

private:
	NFCState state = NFCState::idle;

	State();
	~State();

	void handle_State();
	void handle_Client_TCP();
	void handle_CLI_TCP();

	void handle_Idle();
	void handle_Active();
	void handle_NewUser();

	bool check_For_Tag() const;
	bool is_KnownTag() const;
	void process_KnownTag();
	void register_NewUser();

	HandleTCP client_TCP;
	HandleTCP CLI_TCP;
};
