#pragma once
#include "../Core/Common.h"
#include "./Network.h"

namespace net {
	class TcpConnection;
	using Tcp = boost::asio::ip::tcp;
	using UserPtr = std::shared_ptr<TcpConnection>;
	using ConnectionCallback = std::function<void(const UserPtr&)>;
	using DisconnectionCallback = std::function<void(const uint32)>;
	using MessageCallback = std::function <void(const UserPtr&, std::array<uint8, MAX_BUFFER_SIZE> data, size_t)>;
	using WriteCompleteCallback = std::function<void(const UserPtr&, size_t)>;

	void defaultConnectionCallback(const UserPtr conn)
	{
		std::cout << "[Default]New client connected"  << std::endl;
	}
	void defaultDisconnectionCallback(const uint32 conn)
	{
		std::cout << "[Default]Client disconnected" << std::endl;
	}

	void defaultMessageCallback(const UserPtr& conn, std::array<uint8, MAX_BUFFER_SIZE> data, size_t len)
	{
		std::cout << "[Default]Received: " << len << "bytes" << std::endl;
	}

	void defaultWriteCompleteCallback(const UserPtr& conn, size_t len)
	{
		std::cout << "[Default]Sent: " << len << "bytes" << std::endl;
	}

}