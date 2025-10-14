#include "Reader.h"

#include <iostream>

NFCState Reader::getState() const {
	return state;
}

Reader::Reader(int readerAccessLevel, int port) {
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
	/////////////////////////////////////////////////////////////////////////////
	this->readerAccessLevel = readerAccessLevel;

	////////////////////////////// Init Client TCP //////////////////////////////
	try{
		TcpServer<int> clientTcp(9001);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////// Init CLI TCP ///////////////////////////////
	try{
		TcpServer<int> cliTcp(9002);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	////////////////////////////////////////////////////////////////////////////
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
	auto& pkg = clientTcp.getPackage();
	// Check UID and access level.
	auto user       = users.find(*pkg);
	bool authorized = (user != users.end() && user->second.second >= readerAccessLevel);
	if (authorized) {
		clientTcp.send(static_cast<std::string>("Approved"));
	} else {
		clientTcp.send(static_cast<std::string>("Denied"));
	}

	// Set package to nullptr for handleIdle
	pkg   = nullptr;
	state = NFCState::idle;
}

void Reader::handleCli() {
	auto& pkg             = cliTcp.getPackage();
	std::string prefixStr = "newUser";
	if (pkg->substr(0, prefixStr.size()) == prefixStr) {
		pkg->erase(0, prefixStr.size());
		handleNewUser();
	}
	if (*pkg == "rmUser") {}
	if (*pkg == "getLog") {
		log = getLog();
		cliTcp.send(log);
	}
	pkg   = nullptr;
	state = NFCState::idle;
}

void Reader::handleIdle() {
	// Setting states if packages != nullptr.
	if (cliTcp.getPackage()) state = NFCState::cliActive;
	else if (clientTcp.getPackage()) state = NFCState::clientActive;
}

nlohmann::json Reader::getLog() const {
	return log;
}

void Reader::handleNewUser() {
	auto& clientPkg = clientTcp.getPackage();
	auto& cliPkg    = cliTcp.getPackage();

	std::this_thread::sleep_for(std::chrono::seconds(10));

	if (!clientPkg) {
		return;
	}

	char accessLevel = cliPkg->front();
	cliPkg->erase(0, 1);

	nlohmann::json newUser{
		{"uid", *clientPkg},
		{"name", *cliPkg},
		{"accessLevel", accessLevel}
	};
	users[*clientPkg] = {*cliPkg, accessLevel};

	std::ifstream inJson("users.json");
	nlohmann::json usersJson = nlohmann::json::array();
	if (inJson) {
		inJson >> usersJson;
	}
	inJson.close();

	usersJson.push_back(newUser);

	std::ofstream outJson("users.json");
	outJson << usersJson.dump(4);
	outJson.close();

	clientPkg = nullptr;
	cliPkg    = nullptr;
	state     = NFCState::idle;
}
