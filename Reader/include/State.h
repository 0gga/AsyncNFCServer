#pragma once
#include <string>
#include <thread>
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

	void handle_state();
	void handle_TCP();

	void handle_Idle();
	void handle_Active();
	void handle_New_User();

	bool check_For_Tag() const;
	bool is_Known_Tag() const;
	void process_Known_Tag();
	void register_New_User();

	std::thread client_TCP_t;
	void client_TCP();
	std::thread CLI_TCP_t;
	void CLI_TCP();

	std::pair<int, int>* client_TCP_package = new std::pair<int, int>;
	int* CLI_TCP_package = new int;
};
