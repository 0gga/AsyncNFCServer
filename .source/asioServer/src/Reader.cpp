#include "Reader.h"

#include <iostream>
#include <regex>

NFCState Reader::getState() const {
	return state;
}

Reader::Reader(const int readerAccessLevel, const int clientPort, const int cliPort) :
clientServer(clientPort), cliServer(cliPort), readerAccessLevel(readerAccessLevel) {
	///////////////////////////// Init Client Server /////////////////////////////
	clientServer.start();
	std::cout << "Started Client Server" << std::endl;

	////////////////////////////// Init CLI Server //////////////////////////////
	cliServer.start();
	std::cout << "Started CLI Server" << std::endl;

	////////////////////////////// Read users JSON //////////////////////////////
	std::ifstream file("users.json");
	if (!file) {
		std::cerr << "Failed to open file" << std::endl;
	}
	nlohmann::json json;
	file >> json;

	for (const auto& user : json) {
		if (user.contains("uid") && user.contains("name") && user.contains("accessLevel")) {
			users[user["uid"]] = {user["name"], user["accessLevel"]};
		}
	}
	////////////////////////////// Read users JSON //////////////////////////////
}

Reader::~Reader() {}

void Reader::tick() {
	handleState();
}

void Reader::handleState() {
	switch (state) {
		case NFCState::idle:
			handleIdle();
			break;
		case NFCState::cliActive:
			handleCli();
			break;
		case NFCState::clientActive:
			handleClient();
			break;
	}
}

void Reader::handleClient() {
	auto& pkg = clientServer.read<std::string>();

	// Check UID and access level.
	auto user       = users.find(*pkg);
	bool authorized = (user != users.end() && user->second.second >= readerAccessLevel);

	if (authorized)
		clientServer.write<std::string>("Approved");
	else
		clientServer.write<std::string>("Denied");

	// Set package to nullptr for handleIdle
	pkg   = nullptr;
	state = NFCState::idle;
}

void Reader::handleCli() {
	auto& pkg = cliServer.read<std::string>();

	if (pkg->substr(0, sizeof("newUser")) == "newUser") {
		pkg->erase(0, sizeof("newUser") + 1);
		std::string userData = *pkg;
		addUser(userData);
	}
	if (pkg->substr(0, sizeof("rmUser")) == "rmUser") {
		pkg->erase(0, sizeof("rmUser") + 1);
		std::string userData = *pkg;
		removeUser(userData);
	}
	if (*pkg == "getLog") {
		cliServer.write<nlohmann::json>(getLog());
	}
	pkg   = nullptr;
	state = NFCState::idle;
}

void Reader::handleIdle() {
	// Setting states if packages != nullptr.
	if (cliServer.packageReady())
		state = NFCState::cliActive;
	else if (clientServer.packageReady())
		state = NFCState::clientActive;
}

void Reader::addUser(std::string) {
	auto& clientPkg = clientServer.read<std::string>();
	auto& cliPkg    = clientServer.read<std::string>();

	for (int i{10}; i > 0; i--) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		clientServer.write<std::string>("Waiting for " + (i > 1
															  ? (std::to_string(i) + "Seconds")
															  : (std::to_string(i) + "Second")));
	}

	if (!clientPkg) {
		clientServer.write<std::string>("Failed to add new user - no UID registered");
		return;
	}

	// Parse CLI command for correct syntax
	static const std::regex cliSyntax(
									  R"(^newUser\s+([A-Za-z0-9_]+)\s+([0-9]+)$)");
	std::smatch match;
	if (!std::regex_match(*cliPkg, match, cliSyntax)) {
		clientServer.write<std::string>("Failed to add new user - Incorrect CLI syntax");
		return;
	}

	std::string name        = match[1].str();
	char accessLevel = std::stoi(match[2].str());

	nlohmann::json newUser{
		{"uid", *clientPkg},
		{"name", name},
		{"accessLevel", accessLevel}
	};
	users[*clientPkg] = {name, accessLevel};

	std::ifstream inJson("users.json");
	nlohmann::json usersJson = nlohmann::json::array();
	if
	(inJson) {
		inJson >> usersJson;
	}

	inJson
.
			close();

	usersJson
.
			push_back(newUser);

	std::ofstream outJson("users.json");
	outJson
			<<
			usersJson
.
			dump(
				 4
				);
	outJson
.
			close();

	clientPkg =
			nullptr;
	cliPkg =
			nullptr;
	state = NFCState
			::idle;
}

nlohmann::json Reader::getLog() const {
	return log;
}
