#pragma once
#include "LobbyServer.h"

namespace Lobby {
	class UserManager {
	public:
		UserManager(Lobby::DatabaseHooker& api) { db = &api; }
		void Authorize(Network::UserPtr& user, const String& accountName, const char* password);
		~UserManager();
	private:
		Lobby::DatabaseHooker* db;
	};
}
