#include <Core/GameConstants.h>
#include <Core/Utils/ConfigReader.h>
#include <Core/fwEvent.h>
#include <Network/Tcp/Shared.h>
#include <queue>

namespace Lunia {
	namespace XRated{
		namespace StageServer
		{	
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			typedef std::weak_ptr<User> UserWeakPtr;
			class UserManager {
#pragma region Singleton
			private:
				UserManager();
			public:
				UserManager(const UserManager&) = delete; //anti creation  of a copy
				UserManager& operator= (const UserManager&) = delete; // anti copy
				UserManager(UserManager&&) = delete;
				UserManager& operator=(UserManager&&) = delete;
				static UserManager& GetInstance();
#pragma endregion Singleton
			public:
				~UserManager();
				bool Auth(UserSharedPtr& user, const json& data);

				UserSharedPtr MakeUser(asio::ip::tcp::socket& socket);

				void RemoveUser(UserSharedPtr& user);

				void AuthenticateUser(UserSharedPtr& user);

				UserSharedPtr GetUserByUserId(uint32 userId);

				fwEvent<const UserSharedPtr&>						OnUserConnected;

				fwEvent<const UserSharedPtr&>						OnUserDisconnected;

				fwEvent<const UserSharedPtr&>						OnUserAuthenticated;
			private:
				void RemoveUser(UserSharedPtr& user, AutoLock& _l);

				uint32												m_curUserId = 0;

				std::mutex											m_usersMutex;

				std::unordered_map<uint32, UserSharedPtr>			m_users;
			};

			UserManager& UserManagerInstance();
		}
	}
}
