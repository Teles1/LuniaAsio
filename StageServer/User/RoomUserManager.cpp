#include "RoomUserManager.h"

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomUserManager::RoomUserManager()
			{
			}
			RoomUserManager::RoomUserManager(const RoomUserManager&)
			{
			}
			RoomUserManager::~RoomUserManager()
			{
				AutoLock lock(m_Mtx);
				m_Users.clear();
			}
			uint16 RoomUserManager::NowCount() const
			{
				return (uint16)m_Users.size();
			}
			uint16 RoomUserManager::MaxCount() const
			{
				return m_MaxCount;
			}
			void RoomUserManager::AddUser(UserSharedPtr user)
			{
				AutoLock lock(m_Mtx);
				m_Users.emplace(user->GetId(), user);
			}
			bool RoomUserManager::RemoveUser(UserSharedPtr user)
			{
				AutoLock lock(m_Mtx);
				if (m_Users.find(user->GetId()) == m_Users.end())
					return false;
				m_Users.erase(user->GetId());
				return true;
			}
			bool RoomUserManager::RemoveUser(const uint32& userId)
			{
				AutoLock lock(m_Mtx);
				if (m_Users.find(userId) == m_Users.end())
					return false;
				m_Users.erase(userId);
				return true;
			}
			bool RoomUserManager::DelSerialUser(const uint32& serial)
			{
				AutoLock lock(m_Mtx);
				//CashView stuff
				return true;
			}
			UserSharedPtr RoomUserManager::GetUser(const uint32& userId)
			{
				AutoLock lock(m_Mtx);
				if (m_Users.find(userId) == m_Users.end())
					return NULL;
				return m_Users[userId];
			}
			void RoomUserManager::Update(const float& dt)
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
					user.second->Update(dt);
			}
			void RoomUserManager::BroadcastToAllEnteredUsers(Protocol::IPacketSerializable& value)
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
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
				AutoLock lock(m_Mtx);
				m_Users.clear();
			}
			void RoomUserManager::Flush()
			{
				AutoLock lock(m_Mtx);
				for (auto& user : m_Users)
					user.second->Flush();
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
		}
	}
}