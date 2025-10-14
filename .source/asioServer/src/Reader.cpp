#include "Reader.h"

#include <iostream>
#include <regex>

Reader::Reader(const int readerAccessLevel, const int clientPort, const int cliPort)
: clientServer(clientPort), cliServer(cliPort), readerAccessLevel(readerAccessLevel) {
	///////////////////////////// Init Client Server /////////////////////////////
	clientServer.start();
	std::cout << "Started Client Server" << std::endl;

	////////////////////////////// Init CLI Server //////////////////////////////
	cliServer.start();
	std::cout << "Started CLI Server" << std::endl;

	////////////////////////////// Read users JSON //////////////////////////////
	std::ifstream file("users.json");
	nlohmann::json json = nlohmann::json::array();

	if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
		std::cerr << "Users.json doesn't exist" << std::endl;
		std::ofstream("users.json") << "[]";
		std::cout << "Created empty users.json" << std::endl;
	} else {
		try {
			file >> json;
		} catch (const nlohmann::json::parse_error& e) {
			std::cerr << "Invalid JSON in users.json" << e.what() << std::endl;
			json = nlohmann::json::array();
			std::ofstream("users.json") << "[]";
			std::cout << "Reset invalid users.json\n";
		}
	}

	for (const auto& user : json) {
		if (user.contains("uid") && user.contains("name") && user.contains("accessLevel")) {
			users[user["uid"]] = {user["name"], user["accessLevel"]};
		}
	}
	////////////////////////////// Read users JSON //////////////////////////////
}

Reader::~Reader() {
	stop();
}

void Reader::init() {
	state = ReaderState::Idle;
	handleIdle();
}


void Reader::handleIdle() {
	if (state != ReaderState::Idle)
		return;

	// Setting states if packages != nullptr.
	cliServer.read<std::string>([this](const std::string& pkg) {
		state = ReaderState::Active;
		handleCli();
	});
	clientServer.read<std::string>([this](const std::string& pkg) {
		state = ReaderState::Active;
		handleClient();
	});
}

void Reader::stop() {
	state = ReaderState::Idle;
	TcpServer::stopAll();
}

ReaderState Reader::getState() const {
	return state;
}

void Reader::handleClient() {
	clientServer.read<std::string>([this](const std::string& pkg) {
		const auto user       = users.find(pkg);
		const bool authorized = (user != users.end() && user->second.second >= readerAccessLevel);

		if (authorized)
			clientServer.write<std::string>("Approved");
		else
			clientServer.write<std::string>("Denied");

		state = ReaderState::Idle;
		handleIdle();
	});
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
		state = ReaderState::Idle;
		handleIdle();
	});
}

void Reader::addUser(const std::string& userdata) {
	// Parse CLI command for correct syntax
	static const std::regex cliSyntax(R"(^newUser\s+([A-Za-z0-9_]+)\s+([0-9]+)$)");
	std::smatch match;
	if (!std::regex_match(userdata, match, cliSyntax)) {
		clientServer.write<std::string>("Failed to add new user - Incorrect CLI syntax");
		return;
	}

	std::string name    = match[1].str();
	uint8_t accessLevel = std::stoul(match[2].str());

	clientServer.read<std::string>([this,name,accessLevel](const std::string& uid) {
		nlohmann::json newUser{
			{"uid", uid},
			{"name", name},
			{"accessLevel", accessLevel}
		};
		users[uid] = {name, accessLevel};

		nlohmann::json usersJson = nlohmann::json::array();
		if (std::ifstream in("users.json"); in && in.peek() != std::ifstream::traits_type::eof()) {
			try {
				in >> usersJson;
			} catch (const nlohmann::json::parse_error& e) {
				std::cerr << "JSON parse failed to add user to users.json" << std::endl;
			}
		}
		usersJson.push_back(newUser);
		std::ofstream{"users.json"} << usersJson.dump(4);

		clientServer.write<std::string>("User Added Succesfully");
	});
}

void Reader::removeUser(const std::string&) {}

nlohmann::json Reader::getLog() const {
	return log;
}
