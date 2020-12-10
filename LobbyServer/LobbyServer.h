#pragma once
#include "Network/Tcp/Server.h"
#include "UserRegistry.h"
#include "../Core/GameServer.h"
namespace Lunia {
	typedef GameServer< ScopeLobby > GameServerScope;
	extern std::shared_ptr<GameServerScope> g_gameServer;

	namespace Lobby {
		struct Config {
		public:
			std::string API = "http://localhost:51542/Lobby";
			std::string ServerIp = "127.0.0.1";
			uint16 ServerPort = 15550;
			std::string ServerName = "Lobby_DEV";
			uint16 PingTimeout = 30; //seconds
		}Config;
		struct LobbyServer : public Net::ServerTcp<User> {
			LobbyServer(const char* ip, uint16 port);
			void HandleNewConnection(const asio::error_code& err_code
				, asio::ip::tcp::socket& socket);
		};
	}
}