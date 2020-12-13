#pragma once
#include "Network/Tcp/Server.h"
#include "UserRegistry.h"
#include "../Core/GameServer.h"
#include <Core/Utils/ConfigReader.h>
namespace Lunia {
	typedef GameServer< ScopeLobby > GameServerScope;
	extern std::shared_ptr<GameServerScope> g_gameServer;

	namespace Lobby {
		struct LobbyServer : public Net::ServerTcp<User> {
			LobbyServer(const char* ip, uint16 port);
			void HandleNewConnection(const asio::error_code& err_code
				, asio::ip::tcp::socket& socket);
		};
	}
}