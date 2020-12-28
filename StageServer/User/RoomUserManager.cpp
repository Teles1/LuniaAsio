#include "RoomUserManager.h"
#include "User.h"
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
			uint32 RoomUserManager::NowCount()
			{
				AutoLock lock(m_Mtx);
				return (uint32)m_Users.size();
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
		}
	}
}