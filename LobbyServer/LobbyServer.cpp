#pragma once
#include "./LobbyServer.h"

int main(int argc, char* argv[])
{
	//Load Config
	Lobby::LobbyServer lobbyServer(Lobby::Config.ServerIp.c_str(), Lobby::Config.ServerPort);
	INFO_LOG("{0} Initialized on port {1}", Lobby::Config.ServerName, Lobby::Config.ServerPort);
	lobbyServer.Run();
	return 0;
}