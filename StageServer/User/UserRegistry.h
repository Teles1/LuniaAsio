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
				~UserRegistry() {}
				inline static UserRegistry& GetInstance() {
					static UserRegistry								m_instance;
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

				UserRegistry();
			private:

				uint32												m_curUserId = 0;

				std::mutex											m_usersMutex;

				std::unordered_map<uint32,UserSharedPtr>			m_users;
			};
		}
	}
}

#endif // ! UserRegistry_H_Guard