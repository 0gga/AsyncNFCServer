#include "State.h"

NFCState State::getState() const {
	return state;
}

void State::tick() {
	handle_state();
}

void State::handle_state() {
	switch (state) {
		case NFCState::idle:
			handle_Idle();
			break;
		case NFCState::active:
			handle_Active();
			break;
		case NFCState::newUser:
			handle_New_User();
			break;
	}
}

void State::handle_Idle() {
	if (check_For_Tag())
		state = NFCState::active;
}

void State::handle_Active() {
	if (!checkForTag())
		state = NFCState::idle;
	else if (!isKnownTag())
		state = NFCState::newUser;
}

void State::handle_New_User() {
	registerNewUser();
	state = NFCState::active;
}
