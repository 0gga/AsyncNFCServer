#include "State.h"

NFCState State::getState() const {
	return state;
}

void State::tick() {
	switch (state) {
		case NFCState::idle:
			handle_idle();
			break;
		case NFCState::active:
			handle_active();
			break;
		case NFCState::newUser:
			handle_newUser();
			break;
	}
}

void State::handle_idle() {
	if (checkForTag())
		state = NFCState::active;
}

void State::handle_active() {
	if (!checkForTag())
		state = NFCState::idle;
	else if (!isKnownTag())
		state = NFCState::newUser;
	else
		processKnownTag();
}

void State::handle_newUser() {
	registerNewUser();
	state = NFCState::active;
}
