#pragma once

#include <thread>
#include <memory>
#include <boost/asio.hpp>

class TcpServer {
public:
	TcpServer(int port);
	~TcpServer();

	void start();
	void stop();

	template<typename Rx>
	Rx* read(int) const;

	template<typename Tx>
	void write(const Tx& data) const;

private:
	static boost::asio::io_context io_context;
	static std::thread asyncTcp_t;

	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;

	void acceptConnection();

	void* package = nullptr;
};
