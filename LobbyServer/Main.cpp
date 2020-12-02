#include "../Core/Common.h"
#include "../Network/TcpServer.hpp"
#include <iomanip>
#include "../Core/Serializer/Serializer.h"
#include "Header.h"
#include "LobbyProtocol/LobbyProtocol.h"
#include "Header.h"

inline void onConnection(const net::UserPtr& user) {
	INFO("[{0}] New user Connected", user->getUserID());
}

inline void onDisconnection(const uint32 userID) {
	INFO("[{0}] User Disconnected", userID);
}

inline void onMessage(const net::UserPtr& user, std::array<uint8, MAX_BUFFER_SIZE> data, size_t len) {
	if (len < 6) WARN("[{0}] Message is too small.", user->getUserID());
	std::cout << "[" << user->getUserID() << "Messaged received => " << StringUtil::GetString(data.data(), len) << "\n";
	
	if (len > 4 && (data[2] + (data[3] << 8)) == (uint16)StringUtil::Hash(L"NetStream")) {
		std::cout << "It's NetStream\n";
		Serializer::StreamReader reader = Serializer::StreamReader(&data[4], len - 4);
		uint16 aux = 0;
		reader.Read(aux);
		if ((uint16)StringUtil::Hash(L"Head") == aux) { // pretty please. don't forget to convert to uint16 when hashing =p
			std::cout << "It's Head!\n";
			Lobby::Protocol::Head head;
			head.Deserialize(reader);
			INFO("Version[{0}], Hash[{1}], CheckSum[{2}]",head.ProtocolVersion,head.Hashcode, head.Checksums[0]);
		}
	}
}

int main(int argc, char* argv[])
{
	spdlog::stdout_color_mt("LobbyServer")->set_pattern("[%d/%m/%Y %X] [%^%n%$] %l => %v");
	boost::asio::io_service service;
	uint16 port = 15550;
	net::TcpServer server(service, port);
	server.setConnectionCallback(onConnection);
	server.setDisconnectionCallback(onDisconnection);
	server.setMessageCallback(onMessage);
	INFO("Server started on Port {0}", port);
	service.run();
	return 0;
}
