#pragma once
#include "./LobbyServer.h"
namespace Lobby {

	void onConnection(const Network::UserPtr& user) {
		INFO_LOG("[{0}] New user Connected", user->getUserID());
	}

	void onDisconnection(const uint32 userID) {
		INFO_LOG("[{0}] User Disconnected", userID);
	}

	void onMessage(const Network::UserPtr& user, uint8* data, size_t len) {
		if (len < 6) WARN_LOG("[{0}] Message is too small.", user->getUserID());
		INFO_LOG("[{0}] New message received!", user->getUserID());
		Serializer::StreamReader reader = Serializer::StreamReader(data, len);
		uint16 aux = 0;
		reader.Read(aux);
		if ((uint16)StringUtil::Hash(L"Head") == aux) { // pretty please. don't forget to convert to uint16 when hashing =p
			std::cout << "It's Head!\n";
			Lobby::Protocol::Head head;
			head.Deserialize(reader);
			handleHead(user, head);
		}
		std::cout << std::endl;
	}

	void handleHead(const Network::UserPtr& user, Protocol::Head message)
	{
		Protocol::Head sendPacket;
		if (message.ProtocolVersion != Network::Protocol::Version)
			sendPacket.Result == Protocol::Head::Results::VersionMismatched;
		else
			sendPacket.Result == Protocol::Head::Results::Ok;
		sendPacket.EncryptKey = Math::Random();
		user->SetEncryptKey(sendPacket.EncryptKey);
		sendPacket.ServerTime = DateTime::Now();
		sendPacket.UserIP = user->peerAddress();
		//time to send our first packet!!!!!!!!!!
	}

	void handleAuth(const Network::UserPtr& user, Protocol::Auth message)
	{

	}

}
int main(int argc, char* argv[])
{
	spdlog::set_pattern("[%d/%m/%Y %X] %^%l%$ => %v");
	boost::asio::io_service service;
	uint16 port = 15550;
	Network::TcpServer server(service, port);
	server.setConnectionCallback(Lobby::onConnection);
	server.setDisconnectionCallback(Lobby::onDisconnection);
	server.setMessageCallback(Lobby::onMessage);
	INFO_LOG("Server started on Port {0}", port);
	service.run();
	return 0;
}