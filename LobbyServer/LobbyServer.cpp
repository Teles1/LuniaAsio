#pragma once
#include "LobbyServer.h"
namespace Lobby {
	void LobbyServer::HandleNewConnection(const asio::error_code& err_code, asio::ip::tcp::socket& socket)
	{
		net::UserRegistry::GetInstance()->MakeUser(socket)->HandleRead();
		Logger::GetInstance()->Info("Connection handled by Lobby");
	}
}
int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Logger::GetInstance("LobbyServer");
	//Load Config
	Lobby::LobbyServer lobbyServer(Lobby::Config.ServerIp.c_str(), Lobby::Config.ServerPort);
	Logger::GetInstance()->Info("{0} Initialized on port {1}", Lobby::Config.ServerName, Lobby::Config.ServerPort);
	lobbyServer.Run();
	return 0;
}