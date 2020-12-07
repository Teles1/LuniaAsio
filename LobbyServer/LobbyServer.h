#pragma once
#include "../Network/Tcp/Server.h"
#include "./User.h"

namespace Lobby {
	struct Config {
	public:
		std::string API = "http://localhost:51542";
		std::string ServerIp = "127.0.0.1";
		uint16 ServerPort = 15550;
		std::string ServerName = "Lobby_DEV";
		uint16 PingTimeout = 30; //seconds
	}Config;
	struct LobbyServer : public net::ServerTcp<User>{
		LobbyServer(const char* ip, uint16 port) : ServerTcp(ip, port) 
		{
			
		}
	
	};
}