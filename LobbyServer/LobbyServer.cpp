#pragma once
#include "LobbyServer.h"
#include "Network/Api/Api.h"

#include <Core/Utils/InitFunction.h>
#include <Core/GameServer/GameServer.h>

namespace Lunia {
	namespace Lobby {
		LobbyServer::LobbyServer(const char* ip, uint16 port) : ServerTcp(ip, port)
		{
			Net::Api::ApiUrl = Config::GetInstance().m_ApiBase;
			Net::Api api("AddServer");
			api << Config::GetInstance().m_ServerAddress.ServerPort;
			while (true) {
				auto result = api.RequestApi();
				if (result.errorCode == 0 || result.errorCode == 1) {
					Logger::GetInstance().Info("{0} Initialized on port {1}", Config::GetInstance().m_ServerName, Config::GetInstance().m_ServerAddress.ServerPort);
					break;
				}
				else
					Logger::GetInstance().Error("Could not initiaze server using the API supplied!! {0}", result.errorMessage);
				Sleep(3000);
			}
		}
		void LobbyServer::HandleNewConnection(const asio::error_code& err_code, asio::ip::tcp::socket& socket)
		{
			Net::UserRegistry::GetInstance().MakeUser(socket)->HandleRead();
			Logger::GetInstance().Info("Connection handled by Lobby");
		}
	}
}

int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Logger::GetInstance("LobbyServer");
	Lunia::Config::GetInstance("Config.json");
	Lunia::Net::UserRegistry::GetInstance(Lunia::Config::GetInstance().m_PingTimeout);
	//Load Config
	// Lunia::Lobby::LobbyServer lobbyServer(Lunia::Config::GetInstance().m_ServerAddress.ServerIp.c_str(), Lunia::Config::GetInstance().m_ServerAddress.ServerPort);
	//lobbyServer.Run();

	g_gameServer->RunNetworkIOService(); /* WARN Will lock the thread for as long as it runs */

	return 0;
}

static utils::InitFunction init([]()
{
	/*
	g_gameServer->OnAcceptorCreated.Connect([]()
	{
		std::cout << "Acceptor Created" << std::endl;
	});
	*/

	g_gameServer->Proxy->Say();

	g_gameServer->Proxy->GetClientRegistry().ForAllClients([]()
	{
		std::cout << "Every player got that" << std::endl;
	});

	/*
	g_gameServer->Proxy->PacketListener.Connect([]()
	{

	});
	*/

});