#include "TcpServer.h"
#include <iostream>

boost::asio::io_context TcpServer::io_context;
std::thread TcpServer::asyncTcp_t;

TcpServer::TcpServer(const int port) : acceptor(io_context,
												boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
									   socket(io_context) {}


TcpServer::~TcpServer() {
	stop();
}

void TcpServer::start() {
	running = true;
	acceptConnection();

	if (!asyncTcp_t.joinable()) {
		asyncTcp_t = std::thread([] {
			io_context.run();
		});
	}
}

void TcpServer::stop() {
	if (!running)
		return;
	running = false;

	boost::system::error_code ec;
	acceptor.close(ec);
	socket.close(ec);

	if (ec)
		std::cerr << "Stop error: " << ec.message() << std::endl;
}

void TcpServer::stopAll() {
	if (io_context.stopped())
		return;

	io_context.stop();
	if (asyncTcp_t.joinable())
		asyncTcp_t.join();
}

bool TcpServer::packageReady() {
	return package ? true : false;
}

void TcpServer::acceptConnection() {
	acceptor.async_accept(socket, [this](boost::system::error_code ec) {
		if (!running)
			return;

		if (!ec)
			std::cout << "Client connected on port: " << acceptor.local_endpoint().port() << std::endl;
		else
			std::cerr << "Accept Failed: " << ec.message() << std::endl;
	});
}
