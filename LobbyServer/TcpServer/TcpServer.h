#pragma once
#include <vector>
#include "TcpConnection.h"
#include "Callbacks.h"

namespace net
{
	class TcpServer
	{
	public:
		using Tcp = asio::ip::tcp;

	public:
		TcpServer(const TcpServer&) = delete;
		TcpServer& operator=(const TcpServer&) = delete;
		TcpServer(asio::io_service& ioService, short port);

		void setConnectionCallback(const ConnectionCallback& cb) { }
		void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
		void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

	private:
		void doAccept();
		void newConnection(const asio::error_code& error);

	private:
		Tcp::acceptor	acceptor_;
		Tcp::socket		socket_;

		ConnectionCallback		connectionCallback_;
		MessageCallback			messageCallback_;
		WriteCompleteCallback	writeCompleteCallback_;

		std::vector<std::shared_ptr<TcpConnection>> connections_;
	};

}
