#include "GameServer.h"

static GameServer<ScopeLobby> g_gameServer;

/*	
	It wont execute just yet!
*/

#include "../Core/Utils/InitFunction.h"
static utils::InitFunction init([]()
{
	if (g_gameServer.IsScope<ScopeLobby>())
	{
		ScopeLobby lobbyGameServer = g_gameServer.GetScope<ScopeLobby>();

		lobbyGameServer.A();

		std::cout << "Is ScopeStage : " << (g_gameServer.IsScope<ScopeStage>() == 1) << std::endl;
	}
});
