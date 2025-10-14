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

	void stop();

	ReaderState getState() const;

	bool isRunning() const;
private: // Member Functions
	void handleClient(std::shared_ptr<TcpConnection> connection);
	void handleCli(std::shared_ptr<TcpConnection> connection);

	void addUser(std::shared_ptr<TcpConnection> connection, const std::string&);
	void removeUser(std::shared_ptr<TcpConnection> connection, const std::string&);

	nlohmann::json getLog() const;

private: // Member Variables
	ReaderState state = ReaderState::Idle;
	bool running = true;

	TcpServer clientServer;
	TcpServer cliServer;

	int readerAccessLevel;

	nlohmann::json log;
	std::unordered_map<std::string, std::pair<std::string, int>> users;
};
