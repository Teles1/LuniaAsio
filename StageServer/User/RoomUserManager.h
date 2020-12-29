#pragma once
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			struct RoomUserManager {
				typedef std::shared_ptr<User> UserSharedPtr;
			public:
				RoomUserManager();
				RoomUserManager(const RoomUserManager&);
				~RoomUserManager();
			public:
				uint32 NowCount();
				void AddUser(UserSharedPtr user);
				bool RemoveUser(UserSharedPtr user);
				bool RemoveUser(const uint32& userId);
				UserSharedPtr GetUser(const uint32& userId);
				void Update(const float& dt);
			public:
				std::mutex									m_Mtx;
				std::unordered_map<uint32, UserSharedPtr>	m_Users;
			};
		}
	}
}