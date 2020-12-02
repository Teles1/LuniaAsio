#pragma once
#include "../Core/Common.h"
#include "../Network/TcpServer.hpp"
#include "../Core/Serializer/Serializer.h"
#include "LobbyProtocol/LobbyProtocol.h"
#include "../Core/Utils/Math/Random.h"
namespace Lobby{
	void onConnection(const Network::UserPtr& user);
	void onDisconnection(const uint32 userID);
	void onMessage(const Network::UserPtr& user, uint8* data, size_t len);

#pragma region Handlers
	void handleHead(const Network::UserPtr& user, Protocol::Head message);
	void handleAuth(const Network::UserPtr& user, Protocol::Auth message);
#pragma endregion
}