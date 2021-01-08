#ifndef Main_H
#define Main_H

#pragma once
#include <Core/GameServer/GameServer.h>
#include "GameServer/ServerProxy/ServerProxyLobby.h"

namespace Lunia {
	namespace XRated {
		inline GameServer<ServerProxyLobby>* g_gameServer;
	}
}

#endif // !Main_H