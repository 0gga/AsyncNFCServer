#include "TcpServer.h"
#include <iostream>

boost::asio::io_context TcpServer::io_context;
std::thread TcpServer::asyncTcp_t;

TcpServer::TcpServer(const int port)
	: acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	  socket(io_context) {}


TcpServer::~TcpServer() {
	stop();
}
