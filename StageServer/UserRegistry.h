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
			UserRegistry(const UserRegistry&) = delete; //anti creation  of a copy
			UserRegistry& operator= (const UserRegistry&) = delete; // anti copy
			UserRegistry(UserRegistry&&) = delete;
			UserRegistry& operator=(UserRegistry&&) = delete;
			~UserRegistry() { m_keepAliveLoop = false; m_keepAliveThread.join(); }
			inline static UserRegistry& GetInstance(const uint32& timeout = 30000/*ms*/) {
				static UserRegistry								m_instance(timeout);
				return m_instance;
			}
		public:
			StageServer::UserSharedPtr MakeUser(asio::ip::tcp::socket& socket);

			void RemoveUser(StageServer::UserSharedPtr& user); // outside of this class we shall call this.

			void AuthenticateUser(StageServer::UserSharedPtr& user);

			StageServer::UserSharedPtr GetUserByUserId(uint32 userId);

			fwEvent<const StageServer::UserSharedPtr&>					OnUserConnected;

			fwEvent<const StageServer::UserSharedPtr&>					OnUserDisconnected;

			fwEvent<const StageServer::UserSharedPtr&, const uint32&>	OnUserAuthenticated;

		private:
			void RemoveUser(StageServer::UserSharedPtr& user, AutoLock& _l); //from within the class we should send our AutoLock then finishing deleting the user.

			UserRegistry(const uint32& timeout);

			void NextAlivePingForAll();
		private:
			std::thread												m_keepAliveThread;

			bool													m_keepAliveLoop = true; //set this to false to end the keepAlive

			std::mutex												m_conditionalVar_lock;

			std::condition_variable									m_conditionalVar;

			uint32													m_timeOutTimer;

			uint32													m_curTempUserId = 0xFFFFFFFF;

			uint32													m_curUserId = 0;

			std::mutex												m_usersMutex;

			std::vector<StageServer::UserSharedPtr>					m_users;

			std::unordered_map<uint32, StageServer::UserWeakPtr>	m_usersByUserId;

			std::unordered_map<uint32, StageServer::UserWeakPtr>	m_autorizedUsersByUserId;
		};
		typedef std::shared_ptr<UserRegistry> UserRegPtr;
	}
}

#endif // !UserRegistry_GUARD