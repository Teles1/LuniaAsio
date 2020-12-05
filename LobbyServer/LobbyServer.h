#pragma once
#include "../Core/Core.h"
#include "../Network/stdafx.h"
#include "./LobbyProtocol/LobbyProtocol.h"
#include "../Core/Utils/Math/Random.h"
namespace Lobby{
	struct Config {
	public:
		std::string API = "http://localhost:8080/";
		uint16 Port = 15550;
		std::string ServerName = "Lobby_CA-DEV";
		uint16 PingTimeout = 30; //seconds
	}Config;
	httplib::Client api(Lobby::Config.API.c_str());
	enum Static { 
		SendBufferSize = 40960
	};
	void onConnection(const Network::UserPtr& user);
	void onDisconnection(const uint32 userID);
	void onMessage(const Network::UserPtr& user, uint8* data, size_t len);
	void print_hex_memory(void* mem);
#pragma region User Related
	bool QueryAliveAuth(const Network::UserPtr& user);
	bool CheckAliveAuth(const Network::UserPtr& user);
	void Send(const Network::UserPtr& user, Serializer::ISerializable& packet);
#pragma endregion
#pragma region Handlers
	void handleHead(const Network::UserPtr& user, Protocol::Head message);
	void handleAuth(const Network::UserPtr& user, Protocol::Auth message);
	void handleAlive(const Network::UserPtr& user, Protocol::Alive message);
#pragma endregion
}