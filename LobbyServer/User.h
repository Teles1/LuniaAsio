#pragma once
#include "../Network/Tcp/Client.h"
#include "../Core/Serializer/Serializer.h"
#include "LobbyProtocol/LobbyProtocol.h"

namespace Lobby {
	class User : public net::ClientTcp
	{
	public:
		User(asio::ip::tcp::socket&& socket)
			: ClientTcp(std::move(socket))
			, m_userId(1000)
		{
			std::cout << "User Created!" << std::endl;
		}

		~User()
		{
			std::cout << "User was deleted" << std::endl;
		}

		uint32 GetUserId()
		{
			return m_userId;
		}

		void SetUserId(uint32 userId)
		{
			m_userId = userId;
		}

		void Parse(uint8* buffer, size_t size);
	private:

		uint32 m_userId;
	};
}