#pragma once
// #include "UserRegistry.h"
#include <Network/Tcp/Server.h>
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

#include "Main.h" /* TODO! Remove me */