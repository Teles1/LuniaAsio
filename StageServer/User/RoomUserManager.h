#pragma once
#include "User.h"
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
				uint16 NowCount() const;
				uint16 MaxCount() const;
				void AddUser(UserSharedPtr user);
				bool RemoveUser(UserSharedPtr user);
				bool RemoveUser(const uint32& userId);
				bool DelSerialUser(const uint32& serial);
				UserSharedPtr GetUser(const uint32& userId);
				void Update(const float& dt);
				void BroadcastToAllEnteredUsers(Protocol::IPacketSerializable& value);
				void KickAllUsers();
				void UpdateExpFactor(Logic::ILogic* logic);
				void Clear();
				void Flush();
			private:
				std::mutex m_Mtx;
				std::unordered_map<uint32, UserSharedPtr> m_Users;
				uint16 m_MaxCount = 0;
			};
		}
	}
}