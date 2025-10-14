#pragma once

#include <json.hpp>
#include <thread>
#include <boost/asio.hpp>

#include "TcpConnection.h"

class TcpServer {
public:
	explicit TcpServer(int port);
	~TcpServer();

	void start();
	void stop();
	static void stopAll();

	void onClientConnect(std::function<void(std::shared_ptr<TcpConnection>)> callback);

private: // Member Functions
	void acceptConnection();

private: // Member Variables
	static boost::asio::io_context io_context;
	static std::thread asyncTcp_t;
	static boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;

	boost::asio::ip::tcp::acceptor acceptor;
	bool running = false;
	std::function<void(std::shared_ptr<TcpConnection>)> connectHandler;
};