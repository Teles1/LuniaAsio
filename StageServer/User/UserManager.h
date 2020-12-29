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

				bool AuthenticateUser(const uint32& userId, const uint64& userSerial);

				void RemoveUser(const uint64& userSerial);

				UserSharedPtr GetUserByConnectionId(const uint32& userId);

				UserSharedPtr GetUserByConnectionSerial(const uint64& userSerial);

				fwEvent<const UserSharedPtr&>						OnUserConnected;

				fwEvent<const uint64&>								OnUserDisconnected;

				fwEvent<const UserSharedPtr&>						OnUserAuthenticated;
			private:
				void RemoveUser(const uint64& userSerial, AutoLock& _l);

				uint32												m_curUserId = 0;

				std::unordered_map<uint32, UserSharedPtr>			m_tempUsers; //temp connection id.

				std::mutex											m_usersMutex;

				std::mutex											m_tempUsersMutex;

				std::unordered_map<uint64, UserSharedPtr>			m_users; //SerialId from the db after being authenticated.
			};

			UserManager& UserManagerInstance();
		}
	}
}
