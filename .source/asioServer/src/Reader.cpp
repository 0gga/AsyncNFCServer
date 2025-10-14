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
	cliServer.read<std::string>([this](const std::string& pkg) {
		if (pkg.rfind("newUser", 0) == 0) {
			addUser(pkg);
		} else if (pkg.rfind("rmUser", 0) == 0) {
			removeUser(pkg);
		} else if (pkg == "getLog") {
			cliServer.write<nlohmann::json>(getLog());
		} else {
			cliServer.write<std::string>("Unknown Command");
		}
		state = NFCState::idle;
	});
}

void Reader::handleIdle() {
	// Setting states if packages != nullptr.
	if (cliServer.packageReady())
		state = NFCState::cliActive;
	else if (clientServer.packageReady())
		state = NFCState::clientActive;
}

void Reader::addUser(const std::string& userdata) {
	// Parse CLI command for correct syntax
	static const std::regex cliSyntax(R"(^newUser\s+([A-Za-z0-9_]+)\s+([0-9]+)$)");
	std::smatch match;
	if (!std::regex_match(userdata, match, cliSyntax)) {
		clientServer.write<std::string>("Failed to add new user - Incorrect CLI syntax");
		return;
	}

	std::string name = match[1].str();
	char accessLevel = std::stoul(match[2].str());

	clientServer.read<std::string>([this,name,accessLevel](const std::string& uid) {
		nlohmann::json newUser{
			{"uid", uid},
			{"name", name},
			{"accessLevel", accessLevel}
		};
		users[uid] = {name, accessLevel};

		nlohmann::json usersJson = nlohmann::json::array();
		if (std::ifstream in("users.json"); in) {
			in >> usersJson;
		}
		usersJson.push_back(newUser);
		std::ofstream{"users.json"} << usersJson.dump(4);

		clientServer.write<std::string>("User Added Succesfully");
		state = NFCState::idle;
	});
}

void Reader::removeUser(const std::string&) {}

nlohmann::json Reader::getLog() const {
	return log;
}
