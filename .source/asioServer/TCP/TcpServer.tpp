#include "TcpServer.h"
#include <iostream>

boost::asio::io_context TcpServer::io_context;
std::thread TcpServer::asyncTcp_t;

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
void TcpServer::write(const Tx& data) const {
	boost::asio::async_write(socket, boost::asio::buffer(&data, sizeof(Tx)),
							 [](boost::system::error_code ec, std::size_t) {
								 if (ec)
									 std::cerr << "Write Failed: " << ec.message() << std::endl;
							 });
}
