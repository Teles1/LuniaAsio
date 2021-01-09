#include "RoomUserManager.h"
#include <mmsystem.h>
#include <Core/Utils/ConfigReader.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomUserManager::RoomUserManager() : SaveTimeInMs(ConfigInstance().Get("SynchronizePlayerDataInMsec", 180000))
			{
				
				//m_MailAlarm.m_Interval = ConfigInstance().Get("MailAlarmInterval", 300.0f); // Teles
				m_MailAlarm.Init();
			}
			RoomUserManager::RoomUserManager(const RoomUserManager&) : SaveTimeInMs(ConfigInstance().Get("SynchronizePlayerDataInMsec", 180000))
			{
				//m_MailAlarm.m_Interval = ConfigInstance().Get("MailAlarmInterval", 300.0f); //Teles
				m_MailAlarm.Init();
			}
			RoomUserManager::~RoomUserManager()
			{
				AutoLock uLock(m_Mtx);
				AutoLock pLock(m_PlayersMtx);
				m_Users.clear();
				m_Players.clear();
				m_MailAlarm.Init();
			}
			uint16 RoomUserManager::NowCount(const bool& countSpectators) const
			{
				AutoLock lock(m_Mtx);
				if (countSpectators)
					return (uint16)m_Users.size();
				uint16 count = 0;
				for (auto& user : m_Users)
					if (!user.second->GetCharacterStateFlags().IsSpectator)
						count++;
				return count < 0 ? 0 : count;
			}
			uint16 RoomUserManager::MaxCount() const
			{
				return m_MaxCount;
			}
			void RoomUserManager::AddPlayer(const Serial& serial, UserSharedPtr user)
			{
				AutoLock lock(m_PlayersMtx);
				if (m_Players.find(serial) != m_Players.end())
					LoggerInstance().Exception("Serial duplicated.");
				m_Players.emplace(serial, std::move(user) );
			}
			bool RoomUserManager::RemovePlayer(const Serial& serial)
			{
				AutoLock lock(m_PlayersMtx);
				if (m_Players.find(serial) == m_Players.end())
					return false;
				m_Players.erase(serial);
				return true;
			}
			UserSharedPtr RoomUserManager::GetPlayer(const Serial& serial)
			{
				AutoLock lock(m_PlayersMtx);
				if (m_Players.find(serial) == m_Players.end())
					LoggerInstance().Exception("Player not found.");
				return m_Players[serial];
			}
			std::unordered_map<uint32, UserSharedPtr>& RoomUserManager::GetPlayers()
			{
				return m_Players;
			}
			void RoomUserManager::AddUser(UserSharedPtr user)
			{
				AutoLock lock(m_Mtx);
				m_Users.emplace(user->GetId(), user);
			}
			bool RoomUserManager::RemoveUser(const uint32& userId)
			{
				AutoLock lock(m_Mtx);
				if (m_Users.find(userId) == m_Users.end())
					return false;
				m_Users.erase(userId);
				return true;
			}
			bool RoomUserManager::DoesExist(UserSharedPtr user) const
			{
				AutoLock lock(m_Mtx);
				for (auto& pUser : m_Users)
					if (pUser.second == user)
						return true;
				return false;
			}
			bool RoomUserManager::DoesExist(const uint32& userId) const
			{
				AutoLock lock(m_Mtx);
				for (auto& pUser : m_Users)
					if (pUser.first == userId)
						return true;
				return false;
			}
			bool RoomUserManager::IsEnableStage(const StageLicense& targetStage)
			{
				AutoLock lock(m_Mtx);
				for(auto& user : m_Users)
					if (!user.second->IsAbleToJoinStage(targetStage))
						return false;

				return true;
			}
			bool RoomUserManager::IsJoinningUser()
			{
				AutoLock lock(m_Mtx);
				return m_Users.size() != m_Players.size();
			}
			UserSharedPtr RoomUserManager::GetUser(const uint32& userId)
			{
				AutoLock lock(m_Mtx);
				if (m_Users.find(userId) == m_Users.end())
					return NULL;
				return m_Users[userId];
			}
			std::unordered_map<uint32, UserSharedPtr>& RoomUserManager::GetUsers()
			{
				return m_Users;
			}
			void RoomUserManager::Update(const float& dt)
			{
				AutoLock lock(m_Mtx);
				DWORD nowTime = timeGetTime();
				for (auto& user : m_Users) {
					user.second->Update(dt);
					user.second->AutoSaveUserInfo(nowTime, SaveTimeInMs);
				}
			}
			void RoomUserManager::GetSpectatorNames(std::vector<std::wstring>& names)
			{
				names.clear();
				AutoLock lock(m_Mtx);
				for(auto& user : m_Users)
				{
					if (user.second->GetCharacterStateFlags().IsSpectator)
						names.push_back(user.second->GetCharacterName());
				}
			}
			void RoomUserManager::BroadcastToPlayers(Protocol::IPacketSerializable& value)
			{
				AutoLock lock(m_PlayersMtx);
				for (auto& user : m_Players)
					user.second->Send(value);
			}
			void RoomUserManager::KickAllUsers()
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
					user.second->Close();
			}
			void RoomUserManager::Clear()
			{
				ClearPlayers();
				ClearUsers();
			}
			void RoomUserManager::ClearPlayers()
			{
				AutoLock lock(m_Mtx);
				m_Players.clear();
			}
			void RoomUserManager::ClearUsers()
			{
				AutoLock lock(m_Mtx);
				m_Users.clear();
			}
			void RoomUserManager::BroadcastToSpectators(Protocol::IPacketSerializable& value)
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
					if (user.second->GetCharacterStateFlags().IsSpectator)
						user.second->Send(value);
			}
			void RoomUserManager::BroadcastToSpectators(Protocol::IPacketSerializable& value, const String& ignoreUserName)
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
					if (user.second->GetCharacterStateFlags().IsSpectator && user.second->GetCharacterName() != ignoreUserName)
						user.second->Send(value);
			}
			void RoomUserManager::Flush()
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
					user.second->Flush();
			}
			void RoomUserManager::BroadcastToUsers(Protocol::IPacketSerializable& value)
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
					user.second->Send(value);
			}
			void RoomUserManager::BroadcastToTeam(const uint16& teamNo, Protocol::IPacketSerializable& value)
			{
				AutoLock lock(m_PlayersMtx);
				for (auto& user : m_Players)
					if (user.second->GetTeamNumber() == teamNo)
						user.second->Send(value);
			}
			void RoomUserManager::BroadcastFishingInfo(const bool& fishing, Protocol::IPacketSerializable& value)
			{
				AutoLock lock(m_PlayersMtx);
				for (auto& user : m_Players)
					if (fishing && user.second->IsFishing())
						user.second->Send(value);
					else
						user.second->Send(value);
			}
			void RoomUserManager::UpdateExpFactor(Logic::ILogic* logic)
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users) {
					if (user.second == NULL) {
						LoggerInstance().Warn("RoomUserManager::UpdateExpFactor - Null user in the list");
						continue;
					}
					auto player = user.second->GetPlayer();
					if (player == NULL) { // how would this even happen?
						LoggerInstance().Warn("RoomUserManager::UpdateExpFactor - Null player in the list");
						continue;
					}
					logic->SetPlayerExpFactor(player, user.second->GetExpFactor());
				}
			}
			void RoomUserManager::CheckMailAlaram(UserSharedPtr user)
			{
				AutoLock lock(m_Mtx);
				AutoLock updateLock(m_PlayersMtx);

				//Add to mail alarm list
				std::wstring tmpName = user->GetCharacterName();
				StringUtil::ToLowerCase(tmpName);
				m_MailAlarm.m_UserList[tmpName] = false;

				Net::Api request("Mail/Alarm");
				request << user->GetSerial();
				request.GetAsync();
			}
			void RoomUserManager::MailAlarm::Init()
			{
				m_UserList.clear();
			}

		}
	}
}