#ifndef ClientProxyLobby_H
#define ClientProxyLobby_H
#pragma once
#include <LobbyServer/GameServer/Proxy/ClientProxy.h>
#include <Network/Api/Api.h>

namespace Lunia {
	namespace XRated {
		struct ClientProxyLobby;
		typedef std::shared_ptr<ClientProxyLobby> ClientProxyLobbySharedPtr;

		struct ClientProxyLobby : public ClientProxy
		{
			ClientProxyLobby(asio::ip::tcp::socket&& socket); /* WARN socket is moved a shit ton of times here */

			void SetAsAuthenticated();

			bool IsAuthenticated() const;

			void SetHasSecondPasswordAuthentication();

			bool HasSecondPasswordAuthentication() const;

			bool Ping();

			void CharacterListed(const ClientProxyLobbySharedPtr& user, Net::Answer& answer);

			void AchivementServerFound(const ClientProxyLobbySharedPtr& user, Net::Answer& answer);
		private:

			bool m_isAuthenticated = false;

			bool m_hasSecondPasswordAuthentication = false;
		};
	}
}
#endif // !ClientProxyLobby_H