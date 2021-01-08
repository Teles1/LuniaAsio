#include "Main.h"
#include "Network/Api/Api.h"
#include "PacketHandlers.h"

#include <Core/GameServer/OnGameServerInit.h>

using namespace Lunia;
using namespace Lunia::XRated;
int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Logger::GetInstance("LobbyServer");
	Config::GetInstance("Config_Lobby.json");
	g_gameServer = new GameServer<ServerProxyLobby>(Config::GetInstance().Settings.ServerAddress);
	
	Net::Api::ApiUrl = Config::GetInstance().Settings.ApiUrl;

	Net::Api api("AddServer");
	api << Lunia::Config::GetInstance().Settings.ServerAddress.ServerPort;

	while (true)
	{
		auto result = api.RequestApi();

		if (result.errorCode == 0 || result.errorCode == 1)
		{
			Logger::GetInstance().Info("{0} Initialized on port {1}", Config::GetInstance().Settings.ServerName, Config::GetInstance().Settings.ServerAddress.ServerPort);
			break;
		}
		else
		{
			Logger::GetInstance().Error("Could not initiaze server using the API supplied!! {0}", result.errorMessage);
		}

		Sleep(3000);
	}

	OnGameServerInit::OnInit();

	g_gameServer->RunNetworkIOService(); /* WARN Will lock the thread for as long as it runs */

	return 0;
}