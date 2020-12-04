#pragma once
#include "../Core/Core.h"
#include "../Network/TcpServer.hpp"
#include "../Core/Serializer/Serializer.h"
#include "./LobbyProtocol/LobbyProtocol.h"
#include "../Core/Utils/Math/Random.h"
namespace Lobby{
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