#include "TcpServer.h"
#include <iostream>

TcpServer::TcpServer(const int& port)
: work_guard(make_work_guard(io_context)), acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}


TcpServer::~TcpServer() {
	stop();
}

void TcpServer::start() {
	if (running)
		return;

	running = true;

	io_context.restart();
	acceptConnection();

	asyncThreads_t.clear();
	for (int i = 0; i < threadCount; ++i)
		asyncThreads_t.emplace_back([this] {
			try {
				io_context.run();
			} catch (const std::exception& e) {
				std::cout << "io_context thread exception: " << e.what() << std::endl;
			}
		});
}

void TcpServer::stop() {
	if (!running)
		return;
	running = false;

	io_context.stop();
	for (auto& t : asyncThreads_t)
		if (t.joinable())
			t.join();

	boost::system::error_code ec;
	acceptor.close(ec);

	if (ec)
		std::cout << "Stop error: " << ec.message() << std::endl;
}

void TcpServer::onClientConnect(std::function<void(std::shared_ptr<TcpConnection>)> callback) {
	connectHandler = std::move(callback);
}

void TcpServer::setThreadCount(uint8_t count) {
	if (count <= threadLimit) {
		threadCount = count;
	} else
		std::cout << "Thread count cannot be greater than " << threadLimit << std::endl;
}

void TcpServer::acceptConnection() {
	if (!running)
		return;

	auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
	acceptor.async_accept(*socket, [this,socket](boost::system::error_code ec) {
		if (!running)
			return;
		try {
			if (!ec) {
				const auto connection = std::make_shared<TcpConnection>(std::move(*socket));
				boost::system::error_code ep_ec;
				auto ep = socket->remote_endpoint(ep_ec);
				if (!ep_ec)
					std::cout << "Client connected: " << socket->remote_endpoint() << std::endl;

				if (connectHandler)
					connectHandler(connection);
			} else {
				std::cout << "Accept Failed: " << ec.message() << std::endl;
			}
		} catch (std::exception& e) {
			std::cout << "Exception: " << e.what() << std::endl;
		}
		if (running)
			acceptConnection();
	});
}
