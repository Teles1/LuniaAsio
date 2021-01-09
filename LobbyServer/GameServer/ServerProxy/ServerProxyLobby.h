#ifndef ServerProxyLobby_H
#define ServerProxyLobby_H
#pragma once
#include <LobbyServer/GameServer/Proxy/ServerProxy.h>
#include "../ClientProxy/ClientProxyLobby.h"
namespace Lunia {
	namespace XRated {
		struct ServerProxyLobby : public ServerProxy<ClientProxyLobby>
		{
		public:
			void Say();
		};
	}
}

#endif // !ServerProxyLobby_H
