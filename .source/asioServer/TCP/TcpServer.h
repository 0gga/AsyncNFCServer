#pragma once

#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "json.hpp"

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
	void write(const Tx& data);

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

template<typename Rx>
void TcpServer::read(std::function<void(const Rx&)> handler) {
	auto* buffer = new Rx();

	boost::asio::async_read(socket, boost::asio::buffer(buffer, sizeof(Rx)),
							[this, buffer, handler](boost::system::error_code ec, std::size_t) {
								if (ec) {
									std::cerr << "Read Failed: " << ec.message() << std::endl;
									delete buffer;
									return;
								}
								handler(*buffer);
								delete buffer;
							});
}

template<typename Tx>
void TcpServer::write(const Tx& data) {
	std::string bytes;
	if constexpr (std::is_same_v<Tx, std::string>)
		bytes = data;
	else if constexpr (std::is_same_v<Tx, nlohmann::json>)
		bytes = data.dump();
	else
		static_assert(std::is_same_v<Tx, std::string>||std::is_same_v<Tx, nlohmann::json>,
			"TcpServer::write() exclusively supports std::string or nlohmann::json");

	boost::asio::async_write(socket, boost::asio::buffer(&data, sizeof(Tx)),
							 [](boost::system::error_code ec, std::size_t) {
								 if (ec)
									 std::cerr << "Write Failed: " << ec.message() << std::endl;
							 });
}
