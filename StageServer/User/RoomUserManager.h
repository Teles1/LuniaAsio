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
				uint16 NowCount(const bool& countSpectators = true) const;
				uint16 MaxCount() const;

				void AddUser(UserSharedPtr user);
				void AddPlayer(const uint64& serial, UserSharedPtr user);

				bool RemoveUser(const uint64& userId);
				bool RemovePlayer(const uint64& serial);

				UserSharedPtr GetUser(const uint32& userId);
				UserSharedPtr GetPlayer(const uint64& serial);

				std::unordered_map<uint64, UserSharedPtr>& GetUsers();
				std::unordered_map<uint64, UserSharedPtr>& GetPlayers();

				bool DoesExist(UserSharedPtr user) const; // User
				bool DoesExist(const uint64& userId) const; // User

				bool IsEnableStage(const StageLicense& targetStage);
				bool IsJoinningUser();

				void Update(const float& dt);
				void GetSpectatorNames(std::vector<std::wstring>& names);
				void BroadcastToSpectators(Protocol::IPacketSerializable& value);
				void BroadcastToSpectators(Protocol::IPacketSerializable& value, const String& ignoreUserName);

				void BroadcastToPlayers(Protocol::IPacketSerializable& value);
				void BroadcastToUsers(Protocol::IPacketSerializable& value);

				void BroadcastToTeam(const uint16& teamNo, Protocol::IPacketSerializable& value);
				void BroadcastFishingInfo(const bool& fishing, Protocol::IPacketSerializable& value);

				void KickVoteBroadCasting(Protocol::IPacketSerializable& value, const String& targetName);

				void KickAllUsers();
				void KickTeam(const int teamNo);

				void SetExpPanelty();
				void UpdateExpFactor(Logic::ILogic* logic);
				void LoadStage(const XRated::StageLicense& targetStage);
				void StageLifeInit(const Database::Info::StageGroup* info, bool forceToInitialize = false);

				void SendCashItemViewInfo(const UserSharedPtr user);
				void CashItemView(const UserSharedPtr user, std::pair< uint16, uint16 > flag);
				std::pair<uint16,uint16> GetCashItemViewFlag(const UserSharedPtr user);
				int GetCashItemViewCnt() const;
				void ClearCashItemViewList();

				int GetTeamCnt(int teamNo);

				void MissionClear(bool success, Database::Info::StageGroup* stageGroup, uint16 accessLevel);

				/* quest events */
				void ObjectDestroyed(uint32 id, uint8 team);

				void StageMoving();
				void StageMoving(const std::vector<std::wstring>& userList);

				void GetUserNames(std::vector<std::wstring>& userNames); // param is output
				void GetTeamNames(int teamNo, std::vector<std::wstring>& userNames); // param is output

				uint16 TotalCnt() const;
				uint16 MaxCnt() const;
				uint16 NowCnt() const;

				uint16 CountCurrent(bool includeSpectator = true) const;

				void Flush();
				void Clear();
			public: // mail related
				void CheckMailAlaram(UserSharedPtr user);
			private:
				void ClearPlayers();
				void ClearUsers();
				struct MailAlarm {
					std::map<std::wstring, bool> m_UserList;
					void Init();
				} m_MailAlarm;
			private:
				mutable std::mutex m_Mtx;
				std::unordered_map<uint64, UserSharedPtr> m_Users; //kind of temporary in a sense. This is before the Player gets created. NamedUsers

				mutable std::mutex m_PlayersMtx;
				std::unordered_map<uint64, UserSharedPtr> m_Players;//serial users
				uint16 m_MaxCount = 0;
				const uint32 SaveTimeInMs;
			};
		}
	}
}