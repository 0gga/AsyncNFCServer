#pragma once

#include <thread>
#include <memory>
#include <boost/asio.hpp>

class TcpServer {
public:
	explicit TcpServer(int port);
	~TcpServer();

	void start();
	void stop();
	static void stopAll();

	template<typename Rx>
	void read(std::function<void(const Rx&)> handler);

	template<typename Tx>
	void write(const Tx& data) const;

private: // Member Functions
	void acceptConnection();

private: // Member Variables
	static boost::asio::io_context io_context;
	static std::thread asyncTcp_t;
	static boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;

	bool running = false;
	
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;
};
