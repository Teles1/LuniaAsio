#pragma once
#include "Network/Tcp/Client.h"
#include "Core/Serializer/Serializer.h"
#include "LobbyProtocol/LobbyProtocol.h"

namespace Lobby {
	class User : public net::ClientTcp
	{
	public:
		User(uint32& userId, asio::ip::tcp::socket&& socket)
			: ClientTcp(std::move(socket))
			, m_userId(userId)
		{
			INFO_LOG("User :: Hey, i was created!", GetUserId());
		}

		~User()
		{
			WARN_LOG("Usser :: Hey, i deleted myself, you should check that!", GetUserId());
		}

		const uint32 GetUserId();

		void Parse(uint8* buffer, size_t size);
	private:

		uint32 m_userId;
	};
	typedef std::shared_ptr<User> UserSharedPtr;
}