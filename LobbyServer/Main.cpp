#include "Main.h"

#include <Core/Utils/ConfigReader.h>
#include "Network/Api/Api.h"

int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Logger::GetInstance("LobbyServer");
	Lunia::Config::GetInstance("Config_Lobby.json");

	Lunia::Net::Api::ApiUrl = Lunia::Config::GetInstance().m_ApiBase;

	Lunia::Net::Api api("AddServer");
	api << Lunia::Config::GetInstance().m_ServerAddress.ServerPort;

	while (true)
	{
		auto result = api.RequestApi();

		if (result.errorCode == 0 || result.errorCode == 1)
		{
			Logger::GetInstance().Info("{0} Initialized on port {1}", Lunia::Config::GetInstance().m_ServerName, Lunia::Config::GetInstance().m_ServerAddress.ServerPort);
			break;
		}
		else
		{
			Logger::GetInstance().Error("Could not initiaze server using the API supplied!! {0}", result.errorMessage);
		}

		Sleep(3000);
	}

	g_gameServer->RunNetworkIOService(); /* WARN Will lock the thread for as long as it runs */

	return 0;
}