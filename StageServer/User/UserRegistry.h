#ifndef UserRegistry_H_Guard
#define UserRegistry_H_Guard
#pragma once
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			typedef std::shared_ptr<User> UserSharedPtr;
			typedef std::weak_ptr<User> UserWeakPtr;
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
				UserSharedPtr MakeUser(asio::ip::tcp::socket& socket);

				void RemoveUser(UserSharedPtr& user);

				void AuthenticateUser(UserSharedPtr& user);

				UserSharedPtr GetUserByUserId(uint32 userId);

				fwEvent<const UserSharedPtr&>						OnUserConnected;

				fwEvent<const UserSharedPtr&>						OnUserDisconnected;

				fwEvent<const UserSharedPtr&, const uint32&>		OnUserAuthenticated;

			private:
				void RemoveUser(UserSharedPtr& user, AutoLock& _l);

				UserRegistry(const uint32& timeout);

				void NextAlivePingForAll();
			private:
				std::thread											m_keepAliveThread;

				bool												m_keepAliveLoop = true; //set this to false to end the keepAlive

				std::mutex											m_conditionalVar_lock;

				std::condition_variable								m_conditionalVar;

				uint32												m_timeOutTimer;

				uint32												m_curTempUserId = 0xFFFFFFFF;

				uint32												m_curUserId = 0;

				std::mutex											m_usersMutex;

				std::vector<UserSharedPtr>							m_users;

				std::unordered_map<uint32, UserWeakPtr>				m_usersByUserId;

				std::unordered_map<uint32, UserWeakPtr>				m_autorizedUsersByUserId;
			};
		}
	}
}

#endif // ! UserRegistry_H_Guard