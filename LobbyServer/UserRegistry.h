#pragma once
#include "./User.h"
#include "../Core/fwEvent.h"

struct InitFunction
{
	template<typename T>
	InitFunction(T func)
	{
		func();
	}
};

namespace net
{
	using UserSharedPtr = std::shared_ptr<Lobby::User>;
	class UserRegistry
	{
	public:
		net::UserSharedPtr MakeUser(boost::asio::ip::tcp::socket&& socket);

		UserRegistry() = default;
		void RemoveUser(net::UserSharedPtr& user)
		{

			m_users.erase(user->GetUserId());

			OnUserDisconnected(user);
		}

		UserSharedPtr GetUserByUserId(uint32 userId)
		{
			auto ptr = net::UserSharedPtr();
			auto it = m_users.find(userId);

			if (it != m_users.end())
				ptr = it->second;

			return ptr;
		}
		fwEvent<const net::UserSharedPtr&> OnUserConnected;

		fwEvent<const net::UserSharedPtr&> OnUserDisconnected;

	private:

		uint32 m_curUserId;

		std::unordered_map<uint32, net::UserSharedPtr> m_users;
	};
}



