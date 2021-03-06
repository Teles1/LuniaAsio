#pragma once
#include "LobbyServer.h"
#include <Network/AspApi/AspApi.h>
#include <LobbyServer/Common.h>
#include <Core/Utils/InitFunction.h>
namespace Lunia {
	namespace XRated {
		namespace Lobby {
			LobbyServer::LobbyServer(const char* ip, uint16 port) : ServerTcp(ip, port)
			{
				Http::TextPacket request("Start");
				request << ConfigInstance().Settings.ServerAddress.ServerIp;
				request << ConfigInstance().Settings.ServerAddress.ServerPort;
				request << ConfigInstance().Settings.Capacity;

				DB().Request(request);
				Sleep(500);
			}
			void LobbyServer::HandleNewConnection(const asio::error_code& err_code, asio::ip::tcp::socket& socket)
			{
				UserRegistry().MakeUser(socket)->HandleRead();
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
}

int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Lunia::Logger::GetInstance("LobbyServer");
	Lunia::Config::GetInstance("Config_Lobby.json");
	Lunia::XRated::Lobby::UserRegistry(Lunia::Config::GetInstance().Settings.PingTimeout);
	//Load Config
	Lunia::XRated::Lobby::LobbyServer lobbyServer(Lunia::Config::GetInstance().Settings.ServerAddress.ServerIp.c_str(), Lunia::Config::GetInstance().Settings.ServerAddress.ServerPort);
	lobbyServer.Run();
	return 0;
}
