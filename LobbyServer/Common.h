#ifndef Common_H_Guard
#define Common_H_Guard
#pragma once
#include <Network/UserRegistry.h>
#include <LobbyServer/User.h>
namespace Lunia {
	namespace Lobby {
		inline Net::UserRegistry<Lobby::User>& UserRegistry(const uint32& timeout = 30000) {
			return Net::UserRegistry<Lobby::User>::GetInstance(timeout);
		}
	}
}
#endif // ! Common_H_Guard