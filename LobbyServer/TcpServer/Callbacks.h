#pragma once
#include <memory>
#include <functional>

namespace net
{
	class TcpConnection;
	using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
	using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
	using MessageCallback = std::function <void(const TcpConnectionPtr&, std::array<char, 4098> data, size_t)>;
	using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&,  size_t)>;

	void defaultConnectionCallback(const TcpConnectionPtr& conn);

	void defaultMessageCallback(const TcpConnectionPtr& conn, std::array<char, 4098> data, size_t len);

	void defaultWriteCompleteCallback(const TcpConnectionPtr& conn, size_t len);
}