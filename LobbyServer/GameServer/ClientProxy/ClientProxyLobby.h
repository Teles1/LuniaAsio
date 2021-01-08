#ifndef ClientProxyLobby_H
#define ClientProxyLobby_H
#pragma once
#include <Core/GameServer/Proxy/ClientProxy.h>

namespace Lunia {
	namespace XRated {
		struct ClientProxyLobby : public ClientProxy
		{
			ClientProxyLobby(asio::ip::tcp::socket&& socket); /* WARN socket is moved a shit ton of times here */

			void SetAsAuthenticated();

			bool IsAuthenticated() const;

			void SetHasSecondPasswordAuthentication();

			bool HasSecondPasswordAuthentication() const;

			bool Ping();

		private:

			bool m_isAuthenticated = false;

			bool m_hasSecondPasswordAuthentication = false;
		};
	}
}
#endif // !ClientProxyLobby_H