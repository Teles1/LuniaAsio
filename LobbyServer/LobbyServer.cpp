#pragma once
#include "./LobbyServer.h"
namespace Lobby {
	void LobbyServer::HandleNewConnection(const boost::system::error_code& err_code, boost::asio::ip::tcp::socket& socket)
	{
		INFO_LOG("Connection handled by Lobby");
		net::UserRegistry::GetInstance()->MakeUser(socket)->HandleRead();
	}
}
int main(int argc, char* argv[])
{
	//Load Config
	Lobby::LobbyServer lobbyServer(Lobby::Config.ServerIp.c_str(), Lobby::Config.ServerPort);
	INFO_LOG("{0} Initialized on port {1}", Lobby::Config.ServerName, Lobby::Config.ServerPort);
	lobbyServer.Run();
	return 0;
}