#pragma once
#include "TcpServer.h"

#include <unordered_map>
#include <fstream>

#include "json.hpp"

enum class ReaderState {
	Idle,
	Active
};

class Reader {
public:
	Reader(int aLvl, int clientPort, int cliPort);
	~Reader();

	void init();
	void handleIdle();

	void stop();

	ReaderState getState() const;

private: // Member Functions
	void handleClient();
	void handleCli();

	void addUser(const std::string&);
	void removeUser(const std::string&);

	nlohmann::json getLog() const;

private: // Member Variables
	ReaderState state = ReaderState::Idle;

	TcpServer clientServer;
	TcpServer cliServer;

	int readerAccessLevel;

	nlohmann::json log;
	std::unordered_map<std::string, std::pair<std::string, int>> users;
};
