#pragma once
#include "./LobbyServer.h"
namespace Lobby {

	void onConnection(const Network::UserPtr& user) {
		INFO_LOG("[{0}] New user Connected", user->getUserID());
	}

	void onDisconnection(const uint32 userID) {
		INFO_LOG("[{0}] User Disconnected", userID);
	}
	
	void onWrite(const Network::UserPtr& user, size_t len) {
		INFO_LOG("[{0}] Message sent! Length [{1}]", user->getUserID(), len);
	}

	void onMessage(const Network::UserPtr& user, uint8* data, size_t len) {
		if (len < 6) WARN_LOG("[{0}] Message is too small.", user->getUserID());
		Serializer::StreamReader reader = Serializer::StreamReader(data, len);
		uint16 aux = 0;
		reader.Read(aux);
		INFO_LOG("[{0}] New message received [{1:#04x}]!", user->getUserID(), aux);
		if ((uint16)StringUtil::Hash(L"Head") == aux) { // pretty please. don't forget to convert to uint16 when hashing =p
			std::cout << "It's Head!\n";
			Lobby::Protocol::Head head;
			head.Deserialize(reader);
			handleHead(user, head);
		}
		std::cout << std::endl;
	}
#pragma region User Related
	bool QueryAliveAuth(const Network::UserPtr& user)
	{
		Lobby::Protocol::Alive packet;

		user->alive.Mark = Network::TcpConnection::Marks::AliveCleared;

		Send(user, packet);
		return true;
	}

	bool CheckAliveAuth(const Network::UserPtr& user)
	{
		return false;
	}

	void Send(const Network::UserPtr& user, Serializer::ISerializable& packet)
	{
		StaticBuffer< Static::SendBufferSize > buffer;
		try
		{
			Serializer::StreamWriter writer(buffer);
			writer.Write(packet);
		}
		catch (Exception&) // catch something like buffer overflow
		{
			ERROR_LOG("unable to send",0);
			return;
		}
		user->send(buffer.GetData(), buffer.GetLength());
	}
#pragma endregion User Related
	void handleHead(const Network::UserPtr& user, Protocol::Head message)
	{
		Protocol::Head sendPacket;
		if (message.ProtocolVersion != Constants::Version)
			sendPacket.Result = Protocol::Head::Results::VersionMismatched;
		else
			sendPacket.Result = Protocol::Head::Results::Ok;
		sendPacket.EncryptKey = Math::Random();
		user->SetEncryptKey(sendPacket.EncryptKey);
		sendPacket.ServerTime = DateTime::Now();
		sendPacket.UserIP = user->peerAddress();
		Send(user, sendPacket);
		QueryAliveAuth(user);
	}

	void handleAuth(const Network::UserPtr& user, Protocol::Auth message)
	{

	}

}
void print_hex_memory(void* mem) {
	int i;
	unsigned char* p = (unsigned char*)mem;
	for (i = 1; i < 128; i++) {
		printf("%02x ", p[i-1]);
		//if ((i % 16 == 0))
		//	printf("\n");
	}
	printf("\n");
}
int main(int argc, char* argv[])
{
	/*
	Lobby::Protocol::Head sendPacket;
	sendPacket.Result = Lobby::Protocol::Head::Results::Ok;
	sendPacket.ServerTime = DateTime::Now();
	sendPacket.UserIP = "184.146.62.104";
	sendPacket.EncryptKey = Math::Random();

	StaticBuffer< 40960 > buffer;
	Serializer::StreamWriter writer(buffer);
	writer.Write(sendPacket);
	char* aux = buffer.GetData();
	print_hex_memory(aux);
	return 0;
	*/
	spdlog::set_pattern("[%d/%m/%Y %X] %^%l%$ => %v");
	boost::asio::io_service service;
	uint16 port = 15550;
	try {
		Network::TcpServer server(service, port);
		server.setConnectionCallback(Lobby::onConnection);
		server.setDisconnectionCallback(Lobby::onDisconnection);
		server.setWriteCompleteCallback(Lobby::onWrite);
		server.setMessageCallback(Lobby::onMessage);
		INFO_LOG("Server started on Port {0}", port);
		service.run();
	}
	catch (const boost::system::system_error& ex) {
		ERROR_LOG("Server Exception => {0}", ex.what());
	}
	return 0;
}