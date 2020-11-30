#include <iostream>
#include "Callbacks.h"
#include "TcpConnection.h"

using namespace net;
using namespace std;

void net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
	std::cout << "New client connected from - " << conn->peerIpPort() << std::endl;
}

void net::defaultMessageCallback(const TcpConnectionPtr& conn, std::array<char, 4098> data, size_t len)
{
	std::cout << "received: " << len << "bytes" << std::endl;
}

void net::defaultWriteCompleteCallback(const TcpConnectionPtr& conn, size_t len)
{
	std::cout << "send: " << len << "bytes" << std::endl;
}