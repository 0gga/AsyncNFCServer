#pragma once
#include <string>
#include <vector>

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

	void handle_idle();
	void handle_active();
	void handle_newUser();

	bool checkForTag();
	bool isKnownTag();
	void processKnownTag();
	void registerNewUser();
};
