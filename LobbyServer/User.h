#pragma once
#include "../Network/Tcp/Client.h"
#include "../Core/Serializer/Serializer.h"
#include "LobbyProtocol/LobbyProtocol.h"

namespace Lobby {
	class User : public net::ClientTcp
	{
	public:
		User(uint32 newKey,asio::ip::tcp::socket&& socket)
			: ClientTcp(std::move(socket))
			, m_userId(newKey)
		{
			INFO_LOG("[{}] User Created!", GetUserId());
		}

		~User()
		{
			INFO_LOG("[{}] User was deleted", GetUserId());
		}

		const uint32 GetUserId();

		void Parse(uint8* buffer, size_t size);
	private:

		uint32 m_userId;
	};
	typedef std::shared_ptr<User> UserSharedPtr;
}