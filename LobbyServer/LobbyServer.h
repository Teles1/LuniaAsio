#pragma once
// #include "UserRegistry.h"
#include <Network/Tcp/Server.h>
#include <Core/GameServer.h>
#include <Core/Utils/ConfigReader.h>
#include <LobbyServer/User.h>

namespace Lunia {
	namespace Lobby {
		struct LobbyServer : public Net::ServerTcp<User> {
			LobbyServer(const char* ip, uint16 port);
			void HandleNewConnection(const asio::error_code& err_code
				, asio::ip::tcp::socket& socket);
		};
	}
}

#include <Core/GameServer/GameServer.h>
#include "GameServer/ServerProxy/ServerProxyLobby.h"

inline GameServer<ServerProxyLobby>* g_gameServer = new GameServer<ServerProxyLobby>("127.0.0.1", 15550);