#include "../Core/Common.h"
#include "../Network/TcpServer.hpp"
#include <iomanip>

inline void onConnection(const net::TcpConnectionPtr& conn) {
	std::cout << "[" << conn->getUserID() << "]New User connected => " << conn->peerAddress() << ":" << conn->peerPort() << "\n";
}

inline void onDisconnection(const uint32 userID) {
	std::cout << "[" << userID << "]User disconnected\n";
}

inline void onMessage(const net::TcpConnectionPtr& conn, std::array<uint8, MAX_BUFFER_SIZE> data, size_t len) {
	std::cout << "[" << conn->getUserID() << "Messaged received => " << StringUtil::GetString(data.data(), len) << "\n";
}

int main(int argc, char* argv[])
{
	boost::asio::io_service service;
	uint16 port = 15550;
	net::TcpServer server(service, port);
	server.setConnectionCallback(onConnection);
	server.setDisconnectionCallback(onDisconnection);
	server.setMessageCallback(onMessage);
	printf("Server started on Port %d\n", port);
	service.run();
	return 0;
}
