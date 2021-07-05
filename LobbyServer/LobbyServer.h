#pragma once
#include <Network/Tcp/Server.h>
#include <Core/Utils/ConfigReader.h>
#include <LobbyServer/User.h>

namespace Lunia {
	namespace XRated {
		namespace Lobby {
			static inline Http::DynamicDBConnector<UserSharedPtr>& DB() {
				static Http::DynamicDBConnector<UserSharedPtr> m_dbinstance(ConfigInstance().Settings.ApiUrl, ConfigInstance().Settings.ServerName);
				return m_dbinstance;
			}
			struct LobbyServer : public Net::ServerTcp<User> {
				LobbyServer(const char* ip, uint16 port);
				void HandleNewConnection(const asio::error_code& err_code
					, asio::ip::tcp::socket& socket);
			};
		}
	}
}