#pragma once
#include "./Network.h"
namespace Network {
	class TcpConnection;
	using Tcp = boost::asio::ip::tcp;
	using UserPtr = std::shared_ptr<TcpConnection>;
	using ConnectionCallback = std::function<void(const UserPtr&)>;
	using DisconnectionCallback = std::function<void(const uint32)>;
	using MessageCallback = std::function <void(const UserPtr&, uint8* data, size_t)>;
	using WriteCompleteCallback = std::function<void(const UserPtr&, size_t)>;
	/*
		for the default callbacks the conn-> pointer will be inclomplete. it's referencing a empty class.
	*/
	void defaultConnectionCallback(const UserPtr conn)
	{
		std::cout << "[Default]New client connected" << std::endl;
	}
	void defaultDisconnectionCallback(const uint32 conn)
	{
		std::cout << "[Default]Client disconnected" << std::endl;
	}

	void defaultMessageCallback(const UserPtr& conn, uint8* data, size_t len)
	{
		std::cout << "[Default]Received: " << len << "bytes" << std::endl;
	}

	void defaultWriteCompleteCallback(const UserPtr& conn, size_t len)
	{
		std::cout << "[Default]Sent: " << len << "bytes" << std::endl;
	}
}