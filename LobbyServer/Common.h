#ifndef Common_H_Guard
#define Common_H_Guard
#pragma once
#include <LobbyServer/UserRegistry.h>
#include <LobbyServer/User.h>
namespace Lunia {
	namespace XRated {
		namespace Lobby {
			UserRegistryClass<User>& UserRegistry(const uint32& timeout = 30000);
		}
	}
}
#endif // ! Common_H_Guard