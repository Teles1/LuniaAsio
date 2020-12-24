#pragma once
#include "LobbyServer.h"
#include "Network/Api/Api.h"

#include <Core/Utils/InitFunction.h>
#include <Core/GameServer/GameServer.h>

#include <LobbyServer/Common.h>
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
			Lobby::UserRegistry().MakeUser(socket)->HandleRead();
			Logger::GetInstance().Info("Connection handled by Lobby");
		}

		static utils::InitFunction initFunction([]()
			{
				UserRegistry().OnUserConnected.Connect([](const UserSharedPtr& user)
					{
						Logger::GetInstance().Info("UserRegistry :: OnUserConnected :: userId@{0}", user->GetId());
					});

				UserRegistry().OnUserDisconnected.Connect([](const UserSharedPtr& user)
					{
						Logger::GetInstance().Info("UserRegistry :: OnUserDisconnected :: userId@{0}", user->GetId());

					});

				UserRegistry().OnUserAuthenticated.Connect([](const UserSharedPtr& user, const uint32& oldUserId)
					{
						Logger::GetInstance().Info("UserRegistry :: OnUserAuthenticated :: userId@{0} oldUserId@{1}", user->GetId(), oldUserId);
					});
			});
	}
}

int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Logger::GetInstance("LobbyServer");
	Lunia::Config::GetInstance("Config_Lobby.json");
	Lunia::Lobby::UserRegistry(Lunia::Config::GetInstance().m_PingTimeout);
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