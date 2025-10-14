#pragma once
#include "TcpServer.h"

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
	Reader(int aLvl, int clientPort, int cliPort);
	~Reader();

	NFCState getState() const;
	void tick();

private: // Member Functions
	void handleState();

	void handleClient();
	void handleCli();

	void handleIdle();
	void addUser(std::string);
	void removeUser(std::string);

	nlohmann::json getLog() const;

private: // Member Variables
	NFCState state = NFCState::idle;

	TcpServer clientServer;
	TcpServer cliServer;
	int readerAccessLevel;

	nlohmann::json log;
	std::unordered_map<std::string, std::pair<std::string, int>> users;
};
