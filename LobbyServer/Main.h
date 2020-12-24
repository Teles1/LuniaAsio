#pragma once

#include <Core/GameServer/GameServer.h>
#include "GameServer/ServerProxy/ServerProxyLobby.h"

inline GameServer<ServerProxyLobby>* g_gameServer = new GameServer<ServerProxyLobby>("127.0.0.1", 15550);