#include "Reader.h"

#include <iostream>
#include <regex>

Reader::Reader(const int readerAccessLevel, const int clientPort, const int cliPort)
: clientServer(clientPort), cliServer(cliPort), readerAccessLevel(readerAccessLevel) {
	///////////////////////////// Init Servers /////////////////////////////
	clientServer.onClientConnect([this](std::shared_ptr<TcpConnection> connection) {
		std::cout << "Client Connected\n";
		handleClient(connection);
	});

	cliServer.onClientConnect([this](std::shared_ptr<TcpConnection> connection) {
		std::cout << "CLI Connected\n";
		handleCli(connection);
	});

	clientServer.start();
	cliServer.start();

	running = true;
	std::cout << "Servers started and awaiting clients" << std::endl;
	///////////////////////////// Init Servers /////////////////////////////

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
		if (user.contains("name") && user.contains("uid") && user.contains("accessLevel")) {
			users[user["name"]] = {user["uid"], user["accessLevel"]};
		}
	}
	////////////////////////////// Read users JSON //////////////////////////////
}

Reader::~Reader() {
	stop();
}

void Reader::stop() {
	state = ReaderState::Idle;
	TcpServer::stopAll();
}

ReaderState Reader::getState() const {
	return state;
}

bool Reader::isRunning() const {
	return running;
}

void Reader::handleClient(std::shared_ptr<TcpConnection> connection) {
	state = ReaderState::Active;
	connection->read<std::string>([this, connection](const std::string& pkg) {
		const auto user       = users.find(pkg);
		const bool authorized = (user != users.end() && user->second.second >= readerAccessLevel);

		if (authorized)
			connection->write<std::string>("Approved");
		else
			connection->write<std::string>("Denied");
		state = ReaderState::Idle;
		handleClient(connection);
	});
}

void Reader::handleCli(std::shared_ptr<TcpConnection> connection) {
	state = ReaderState::Active;
	connection->read<std::string>([this, connection](const std::string& pkg) {
		if (pkg.rfind("newUser", 0) == 0) {
			addUser(connection, pkg);
		} else if (pkg.rfind("rmUser", 0) == 0) {
			removeUser(connection, pkg);
		} else if (pkg == "getLog") {
			connection->write<nlohmann::json>(getLog());
		} else if (pkg == "shutdown") {
			connection->write<std::string>("Shutting Down...");
			running = false;
			TcpServer::stopAll();
			return;
		} else {
			connection->write<std::string>("Unknown Command");
		}
		state = ReaderState::Idle;
		handleCli(connection);
	});
}

void Reader::addUser(std::shared_ptr<TcpConnection> connection, const std::string& userdata) {
	// Parse CLI command for correct syntax
	static const std::regex cliSyntax(R"(^newUser\s+([A-Za-z0-9_]+)\s+([0-9]+)$)");
	std::smatch match;
	if (!std::regex_match(userdata, match, cliSyntax)) {
		connection->write<std::string>("Failed to add new user - Incorrect CLI syntax");
		return;
	}

	std::string name    = match[1].str();
	uint8_t accessLevel = std::stoul(match[2].str());

	connection->read<std::string>([this, name, accessLevel, connection](const std::string& uid) {
		nlohmann::json usersJson = nlohmann::json::array();
		if (std::ifstream in("users.json"); in && in.peek() != std::ifstream::traits_type::eof()) {
			try {
				in >> usersJson;
			} catch (const nlohmann::json::parse_error& e) {
				std::cerr << "JSON parse failed to add user to users.json" << e.what() << std::endl;
			}
		}
		nlohmann::json newUser{
			{"name", name},
			{"uid", uid},
			{"accessLevel", accessLevel}
		};

		users[name] = {uid, accessLevel};
		usersJson.push_back(newUser);

		std::ofstream{"users.json"} << usersJson.dump(4);

		connection->write<std::string>("User Added Succesfully");
	});
}

void Reader::removeUser(std::shared_ptr<TcpConnection> connection, const std::string& userdata) {
	// Parse CLI command for correct syntax
	static const std::regex cliSyntax(R"(^rmUser\s+([A-Za-z_]+)$)");
	std::smatch match;
	if (!std::regex_match(userdata, match, cliSyntax)) {
		connection->write<std::string>("Failed to add new user - Incorrect CLI syntax");
		return;
	}

	std::string name = match[1].str();

	if (users.erase(name) == 0) {
		std::cout << "User not found in memory" << std::endl;
		return;
	}

	nlohmann::json usersJson = nlohmann::json::array();
	try {
		std::ifstream in("users.json");
		if (in && in.peek() != std::ifstream::traits_type::eof())
			in >> usersJson;
		else
			usersJson = nlohmann::json::array();
	} catch (const nlohmann::json::parse_error& e) {
		std::cerr << "JSON parse failed to remove user from users.json" << e.what() << std::endl;
		usersJson = nlohmann::json::array();
	}

	for (auto it = usersJson.begin(); it != usersJson.end(); ++it) {
		if (it->contains("name") && (*it)["name"] == name) {
			usersJson.erase(it);
			break;
		}
	}

	std::ofstream{"users.json"} << usersJson.dump(4);
	connection->write<std::string>("User Removed Succesfully");
}

nlohmann::json Reader::getLog() const {
	return log;
}
