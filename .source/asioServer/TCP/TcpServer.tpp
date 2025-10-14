#include "TcpServer.h"
#include <iostream>

boost::asio::io_context TcpServer::io_context;
std::thread TcpServer::asyncTcp_t;

template<typename Rx>
Rx*& TcpServer::read() {
	auto* buffer = new Rx();
	package      = buffer;

	boost::asio::async_read(socket, boost::asio::buffer(package, sizeof(Rx)),
							[this,buffer](boost::system::error_code ec, std::size_t) {
								if (ec) {
									std::cerr << "Read Failed: " << ec.message() << std::endl;
									delete buffer;
									package = nullptr;
								} else
									std::cout << "Recieved " << sizeof(Rx) << " Bytes" << std::endl;
							});
	return buffer;
}

template<typename Tx>
void TcpServer::write(const Tx& data) const {
	boost::asio::async_write(socket, boost::asio::buffer(&data, sizeof(Tx)),
							 [](boost::system::error_code ec, std::size_t) {
								 if (ec)
									 std::cerr << "Write Failed: " << ec.message() << std::endl;
							 });
}
