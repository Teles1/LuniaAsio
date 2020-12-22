#pragma once

#include <Core/GameServer/Proxy/ServerProxy.h>

#include "../ClientProxy/ClientProxyLobby.h"

struct ServerProxyLobby : public ServerProxy<ClientProxyLobby>
{
public:
	void Say()
	{
		std::cout << "ServerProxyLobby :: Say" << std::endl;
	};
};

