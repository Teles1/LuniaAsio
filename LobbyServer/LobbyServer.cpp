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
		Serializer::StreamReader reader = Serializer::StreamReader(data, len);
		uint16 aux = 0;
		reader.Read(aux);
		if (Protocol::Head::TypeHash == aux) {
			INFO_LOG("[{0}] Head Received!", user->getUserID());
			Lobby::Protocol::Head packet;
			packet.Deserialize(reader);
			handleHead(user, packet);
		}
		else if (Protocol::Alive::TypeHash == aux) {
			INFO_LOG("[{0}] Alive Received!", user->getUserID());
			Lobby::Protocol::Alive packet;
			packet.Deserialize(reader);
			handleAlive(user, packet);
		}
		else if(Protocol::Auth::TypeHash == aux){
			INFO_LOG("[{0}] Auth Received!", user->getUserID());
			Lobby::Protocol::Auth packet;
			packet.Deserialize(reader);
			handleAuth(user, packet);
		}
		else {
			WARN_LOG("[{0}] Unkown Packet Received [{1:04X}]",user ->getUserID(), aux);
			print_hex_memory(data);
		}
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
		user->SetKey(sendPacket.EncryptKey);
		//INFO_LOG("CryptoKey[{0}]", sendPacket.EncryptKey);
		sendPacket.ServerTime = DateTime::Now();
		sendPacket.UserIP = user->peerAddress();
		Send(user, sendPacket);
		QueryAliveAuth(user);
	}

	void handleAuth(const Network::UserPtr& user, Protocol::Auth message)
	{

	}
	void handleAlive(const Network::UserPtr& user, Protocol::Alive message)
	{
		Network::TcpConnection::Alive::AliveData answer;
		answer.index = message.Index;
		answer.value1 = message.Value1;
		answer.value2 = message.Value2;
		answer.value3 = message.Value3;
		user->UpdateAliveAuth(answer);
	}

	void print_hex_memory(void* mem) {
		int i;
		unsigned char* p = (unsigned char*)mem;
		for (i = 1; i < 128; i++) {
			printf("%02x ", p[i - 1]);
			//if ((i % 16 == 0))
			//	printf("\n");
		}
		printf("\n");
	}
	void InitializeApi() {
		try {
			StringUtil::StringBuilder call;
			//send lobby server to the api.
			call << "AddServer/" << Lobby::Config.ServerName << "?ServerIp=" << Lobby::Config.ServerIp <<
				"&ServerPort=" << Lobby::Config.ServerPort;
			[](std::string& in) {
				while (!Api.Send(in)) {
					Sleep(1000);
				}
			}(call.GetString());
		}
		catch (...) {
			INFO_LOG("Exception thrown Initializing server to db");
		}
	}
}

int main(int argc, char* argv[])
{
	Lobby::InitializeApi();
	spdlog::set_pattern("[%d/%m/%Y %X]%^%l%$ => %v");
	//INFO_LOG("[{0}] Initialized!", Lobby::Config.ServerName);
	boost::asio::io_service service;
	uint16 port = 15550;
	try {
		Network::TcpServer server(service, port);
		server.setConnectionCallback(Lobby::onConnection);
		server.setDisconnectionCallback(Lobby::onDisconnection);
		server.setWriteCompleteCallback(Lobby::onWrite);
		server.setMessageCallback(Lobby::onMessage);
		INFO_LOG("{0} started on Port {1}", Lobby::Config.ServerName, port);
		service.run();
	}
	catch (const boost::system::system_error& ex) {
		ERROR_LOG("Server Exception => {0}", ex.what());
		return 0;
	}
	return 0;
}