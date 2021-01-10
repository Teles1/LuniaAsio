#pragma once
#include <Core/GameConstants.h>
#include <Logic/Logic.h>
#include <Network/Api/Api.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;

			typedef std::map<const std::wstring, UserSharedPtr>	stringUserMap;
			typedef std::map<unsigned int, UserSharedPtr>		serialUserMap;


			class RoomUserManager
			{
			public:
				RoomUserManager();
				RoomUserManager(const RoomUserManager&);
				~RoomUserManager();

			public:
				void AddNameUser(const std::wstring& name, UserSharedPtr user);
				void AddSerialUser(unsigned int serial, UserSharedPtr user);
				bool DelNameUser(const std::wstring& name);
				bool DelSerialUser(unsigned int serial);

				void Clear();
				void ClearNameUser();
				void ClearSerialUser();

				UserSharedPtr GetUser(const std::wstring& name);
				UserSharedPtr GetUser(unsigned int serial);

				bool IsExist(UserSharedPtr user);
				bool IsExist(const std::wstring& userName);

				bool IsEnableStage(const StageLicense& targetStage);
				bool IsJoinningUser();

				void Update(float dt);
				void UpdateMailAlarm(float dt);
				void GetSpectatorNames(std::vector< std::wstring >& names);
				void BroadcastToSpectators(Serializer::ISerializable& value);
				void BroadcastToSpectators(Serializer::ISerializable& value, const std::wstring& ignoreUserName);

				void BroadcastToSerialUsers(Serializer::ISerializable& value);
				void BroadcastToSerialUsers(Serializer::ISerializable& value, unsigned int ignoreUserSerial, const std::wstring& ignoreUserName);

				void BroadcastToAchivementUsers(Serializer::ISerializable& value);
				void BroadcastToAllEnteredUsers(Serializer::ISerializable& value);

				void BroadcastToTeam(int teamNo, Serializer::ISerializable& value);
				void BroadcastFishingInfo(bool fishing, Serializer::ISerializable& value);
				void Flush();

				void KickVoteBroadCasting(Serializer::ISerializable& value, std::wstring& targetName);
				void KickAllUsers();
				void KickTeam(int teamNo);
				//void SetExpFactor(float factor);
				void SetExpPanelty();
				void UpdateExpFactor(Logic::ILogic* logic);
				void LoadStage(const StageLicense& targetStage);
				void StageLifeInit(const Database::Info::StageGroup* info, bool forceToInitialize = false);
				void SendCashItemViewInfo(UserSharedPtr user);

				void CashItemView(UserSharedPtr user, std::pair< uint16, uint16 > flag);
				std::pair< uint16, uint16 > GetCashItemViewFlag(const UserSharedPtr user);
				int GetCashItemViewCnt() const;
				void ClearCashItemViewList();

				int GetTeamCnt(int teamNo);

				void MissionClear(bool success,Database::Info::StageGroup* stageGroup, uint16 accessLevel);

				/* quest events */
				void ObjectDestroyed(uint32 id, uint8 team);

				void StageMoving();
				void StageMoving(const std::vector<std::wstring>& userList);

				void GetUserNames(std::vector<std::wstring>& userNames); // param is output
				void GetTeamNames(int teamNo, std::vector<std::wstring>& userNames); // param is output

				///
				stringUserMap& GetNameUsers();
				serialUserMap& GetSerialUsers();

				int TotalCnt() const;
				int MaxCnt() const;
				int NowCnt() const;

				int	CountCurrent(bool includeSpectator = true)	const;
				void CheckMailAlaram(UserSharedPtr user);

			private:
				struct UserFinder {
					String userName;
					UserFinder(const String& name);
					bool operator()(std::map<const std::wstring, UserSharedPtr>::value_type& i);
				};

			private:
				stringUserMap nameUsers;	// All of users on room
				serialUserMap serialUsers;	// Active users on logic

				std::map< XRated::Serial /*player serial*/, std::pair< uint16, uint16 > /*cash item flag*/> cashItemViewList; 
				uint32 SaveTimeInMs;
				size_t totalCnt;
				size_t maxCnt; // This may be significant at Square

				mutable std::mutex cs;

				mutable std::mutex updateCS;

			private: //Mail
				struct MailAlarm {
					float INTERVAL;
					float elapsedTime;
					std::map<std::wstring, bool >	userlist;
					void Init();
					bool Update(float dt);
				} mailAlarm;
			public:
				void MailAlarmed(const UserSharedPtr&, const Net::Answer& answer);
			};

		}
	}
}