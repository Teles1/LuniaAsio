#ifndef UserRegistry_GUARD
#define UserRegistry_GUARD

#pragma once
#include "User.h"
namespace Lunia {
	namespace Net
	{
		class UserRegistry
		{
		public:
			static std::mutex								m_lock;
			static std::shared_ptr<UserRegistry>			m_instance;
		public:
			UserRegistry(const UserRegistry&) = delete; //anti creation  of a copy
			UserRegistry& operator= (const UserRegistry&) = delete; // anti copy
			UserRegistry(UserRegistry&&) = delete;
			UserRegistry& operator=(UserRegistry&&) = delete;
			~UserRegistry() { }
			static std::shared_ptr<UserRegistry>& GetInstance();
		public:
			Lobby::UserSharedPtr MakeUser(asio::ip::tcp::socket& socket);

			void RemoveUser(Lobby::UserSharedPtr& user);

			void AuthenticateUser(Lobby::UserSharedPtr& user);

			Lobby::UserSharedPtr GetUserByUserId(uint32 userId);

			fwEvent<const Lobby::UserSharedPtr&>				OnUserConnected;

			fwEvent<const Lobby::UserSharedPtr&>				OnUserDisconnected;

			fwEvent<const Lobby::UserSharedPtr&, const uint32&>                OnUserAuthenticated;

		private:
			UserRegistry() { };
		private:

			uint32 m_curTempUserId = 0xFFFFFFFF;

			uint32 m_curUserId = 0;

			std::mutex m_usersMutex;
			std::vector<Lobby::UserSharedPtr> m_users;

			std::unordered_map<uint32, Lobby::UserWeakPtr> m_usersByUserId;

			std::unordered_map<uint32, Lobby::UserWeakPtr> m_autorizedUsersByUserId;
		};
		typedef std::shared_ptr<UserRegistry> UserRegPtr;
	}
}

#endif // !UserRegistry_GUARD