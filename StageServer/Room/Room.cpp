#include "Room.h"
#include <StageServer/User/UserManager.h>
#include <Core/ErrorDefinition.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			Room::Room(const int& index)
			{
				logic = Logic::CreateLogic();
			}

			bool Room::Update(const float& dt)
			{
				{
					//Check weather the room is empty or not.
				}
				{
					AutoLock lock(m_Mtx);
					if (!m_Active)
						return true;
					if (!m_Loading) {
						if (m_LastThreadIndex != m_ThreadIndex)
						{
							Logger::GetInstance().Info(L"roomCs(index:{0}) updating thread is changed({1} to {2})", m_RoomIndex, m_LastThreadIndex, m_ThreadIndex);
							m_LastThreadIndex = m_ThreadIndex;
						}
						assert(m_ThreadIndex != -1);
						logic->Update(dt);
					}
				}
				CheckLoadingTime(dt);

				return true;
			}
			void Room::SetThreadIndex(const int& i)
			{
				AutoLock lock(m_Mtx);
				m_Active = false; // kind of hack - because when SetThreadIndex is set, logic should not be updated before initialize 
				m_ThreadIndex = i;
			}

			void Room::CheckLoadingTime(const float& dt)
			{
				if (GetRoomKind() == RoomKind::Pvp && pvp.SyncEnteredUserCnt == pvp.SyncStartUserCnt) {
					m_LoadingTime += dt;
					if (m_LoadingTime > 180.0f)
						; // kick all users using user manager
				}
			}

			RoomKind Room::GetRoomKind()
			{
				return m_RoomKind;
			}

			const Logic::ILogic::UpdateInfo& Room::GetLogicUpdateInfo()
			{
				return logic->GetUpdateInfo();
			}

			const int& Room::GetIndex()
			{
				return m_RoomIndex;
			}

			bool Room::JoinUser(UserSharedPtr user, const std::string& roomPass)
			{
				if (m_NowCampfire) {
					Protocol::Error sendPacket;
					sendPacket.errorcode = Errors::UnableToJoinStage;
					user->Send(sendPacket);
					Logger::GetInstance().Error("Could not join user={0} to the room={1}", user->GetId(), this->GetIndex());
					return false;
				}
				/*if (!SetStage(user->GetCurrentStage(), roomPass, user->GetRoomActivateSerial(), user->GetName())) {

				}*/
				return false;
			}

			bool Room::SetStage(StageLicense& targetStage, const std::string& roomPass, int64 pActivateSerial, const std::wstring& userName)
			{
				AutoLock lock(m_Mtx);

				return false;
			}
		}
	}
}