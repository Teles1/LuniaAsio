#ifndef ServerProxyLobby_H
#define ServerProxyLobby_H
#pragma once
#include <Core/GameServer/Proxy/ServerProxy.h>
#include "../ClientProxy/ClientProxyLobby.h"
namespace Lunia {
	struct ServerProxyLobby : public ServerProxy<ClientProxyLobby>
	{
	public:
		void Say();
	};
}

#endif // !ServerProxyLobby_H
