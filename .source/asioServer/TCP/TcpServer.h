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
	Rx*& read();

	template<typename Tx>
	void write(const Tx& data) const;

	bool packageReady();
private:
	static boost::asio::io_context io_context;
	static std::thread asyncTcp_t;

	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;

	void acceptConnection();

	static inline int activeServers{0};
	bool running  = false;
	void* package = nullptr;
};
