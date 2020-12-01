#pragma once
#include <vector>
#include "../Core/Common.h"
#include "./Network.h"
#include "TcpConnection.hpp"

namespace net
{
	class TcpServer
	{
	public:
		TcpServer(const TcpServer&) = delete;
		TcpServer& operator=(const TcpServer&) = delete;
		TcpServer(boost::asio::io_service& ioService, uint16 port) :
			acceptor_(ioService, Tcp::endpoint(Tcp::v4(), port)),
			socket_(ioService),
			connectionCallback_(defaultConnectionCallback),
			disconnectionCallback_(defaultDisconnectionCallback),
			messageCallback_(defaultMessageCallback),
			writeCompleteCallback_(defaultWriteCompleteCallback)
		{
			doAccept();
		}

		void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
		void setDisconnectionCallback(const DisconnectionCallback& cb) { disconnectionCallback_ = cb; }
		void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
		void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

	private:
		void doAccept()
		{
			acceptor_.async_accept(socket_, std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
		}
		void newConnection(const boost::system::error_code& error)
		{

			if (!error)
			{
				auto conn = std::make_shared<TcpConnection>(std::move(socket_));
				conn->setMessageCallback(messageCallback_);
				conn->setWriteCompleteCallback(writeCompleteCallback_);

				conn->start(userID);
				userID++;
				connections_.push_back(conn);
				connectionCallback_(conn);

				doAccept();
			}
			else {
				std::cout << "Could not connect => " << error.message() << std::endl;
			}
		}

	private:
		Tcp::acceptor	acceptor_;
		Tcp::socket		socket_;

		ConnectionCallback		connectionCallback_;
		DisconnectionCallback	disconnectionCallback_;
		MessageCallback			messageCallback_;
		WriteCompleteCallback	writeCompleteCallback_;

		std::vector<std::shared_ptr<TcpConnection>> connections_;
		uint32 userID = 1000;
	};

}
