#ifndef UserRegistry_GUARD
#define UserRegistry_GUARD

#pragma once
#include "User.h"

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
	class UserRegistry
	{
	public:
		static std::mutex								m_lock;
		static std::shared_ptr<UserRegistry>			m_instance;
	public:
		UserRegistry(const UserRegistry&) = delete; //anti creation  of a copy
		UserRegistry& operator= (const UserRegistry&) = delete; // anti copy
		~UserRegistry() { std::cout << "User regestry destroyed" << std::endl; }
		static std::shared_ptr<UserRegistry>& GetInstance();
	public:
		Lobby::UserSharedPtr MakeUser(asio::ip::tcp::socket& socket);

		void RemoveUser(Lobby::UserSharedPtr& user);

		Lobby::UserSharedPtr GetUserByUserId(uint32 userId);
		
		fwEvent<const Lobby::UserSharedPtr&>				OnUserConnected;

		fwEvent<const Lobby::UserSharedPtr&>				OnUserDisconnected;

	private:
		UserRegistry() : m_curUserId(1000) { INFO_LOG("UserRegistry Instanciated"); }
	private:
		uint32											m_curUserId;

		std::unordered_map<uint32, Lobby::UserSharedPtr>	m_users;
	};
	typedef std::shared_ptr<UserRegistry> UserRegPtr;
}

#endif // !UserRegistry_GUARD