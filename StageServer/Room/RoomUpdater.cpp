#include "RoomUpdater.h"
#include <StageServer/User/UserManager.h>
#include <Core/Utils/StringUtil/General.h>
#include <mmsystem.h>
#include <StageServer/Room/RoomUpdateManager.h>
#include <StageServer/Room/Room.h>
#include <StageServer/User/User.h>
//#define DEBUG
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomUpdater::RoomUpdater(uint16 index)
				: Runnable(fmt::format(L"Room Updator Index {0}", index).c_str())
				, m_Index(index)
			{
				Logger::GetInstance().Info("RoomUpdater::RoomUpdater({0})", index);
				StartThread();
			}

			RoomUpdater::~RoomUpdater() {
				StopThread();
				Logger::GetInstance().Info("RoomUpdater::~RoomUpdater()");
			}

			void RoomUpdater::Run() {
				m_OldTime = timeGetTime();

				m_Dt = m_Timer.GetTimeDelta();
				UpdateRooms(m_Dt);
				m_NowTime = timeGetTime();
				m_UpdateTime = m_NowTime - m_OldTime;
				m_LoopTime += m_UpdateTime;
				++m_UpdateCount;
				if (m_UpdateTime > 50)
				{
					Logger::GetInstance().Warn("RoomUpdater::Run() - Caution : updateTime({0}) > 50", m_UpdateTime );
					ShowLogicPerformance();
				}
				else
					Sleep(1);
				m_PassedTime += m_Dt;
#ifdef DEBUG
				if (m_PassedTime > 5.0f) {
					Logger::GetInstance().Info("[Info Perfomance] TotalTime={0}, Loop={1}, lps={2}", m_LoopTime, m_UpdateCount, (m_UpdateCount * 1000) / (m_LoopTime == 0 ? 1 : m_LoopTime));
				}
#endif // DEBUG
			}

			void RoomUpdater::AddRoom(RoomSharedPtr room)
			{
				AutoLock lock(m_Mtx);

				m_Rooms.insert(room);
				room->SetThreadIndex(m_Index);
			}

			void RoomUpdater::DelRoom(RoomSharedPtr room)
			{
				AutoLock lock(m_Mtx);

				room->SetThreadIndex(-1);
				m_Rooms.erase(room);
			}

			bool RoomUpdater::JoinUser(RoomSharedPtr room, UserSharedPtr user, const std::string& roomPass)
			{
				AutoLock lock(m_Mtx);

				auto itr = m_Rooms.find(room);
				if (itr != m_Rooms.end())
					return (*itr)->JoinUser(user, roomPass);
				LoggerInstance().Error("RoomUpdater::JoinUser - Unable the specified room");
				return false;
			}

			bool RoomUpdater::PartUser(RoomSharedPtr room, UserSharedPtr user) {
				AutoLock lock(m_Mtx);

				auto itr = m_Rooms.find(room);
				if (itr != m_Rooms.end())
					return (*itr)->PartUser(user);
				return false;
			}

			void RoomUpdater::UpdateExpFactor() {
				AutoLock lock(m_Mtx);

				for (auto& x: m_Rooms)
				{
					//(*itr)->ChangeExpFactor( expFactor );
					x->UpdateExpFactor();
				}
			}

			void RoomUpdater::NoticeHolidayEvent(const uint32& eventId, bool start) {
				AutoLock lock(m_Mtx);

				for (auto& x : m_Rooms)
					x->NoticeHolidayEvent(eventId, start);
			}

			uint16 RoomUpdater::GetRoomCnt() {
				return uint16(m_Rooms.size());
			}

			void RoomUpdater::UpdateRooms(float dt)
			{
				AutoLock lock(m_Mtx);

				for (auto& room : m_Rooms)
				{
					if ( room->Update(dt) == false)
					{
						room->SetThreadIndex(-1);
						m_Rooms.erase(room);
					}
				}
			}
			void RoomUpdater::ShowLogicPerformance()
			{
				AutoLock lock(m_Mtx);
				float totalTime = 0;
				int updatedRoom = 0;
				for (auto& room : m_Rooms)
				{
					const Logic::ILogic::UpdateInfo& info = room->GetLogicUpdateInfo();
					totalTime += static_cast<float>(info.processCommandTime);
					totalTime += static_cast<float>(info.updateTime);
					Logger::GetInstance().Info("[Logic] RoomIndex={0} Update={1} Cmd={2}/{3} Update={4}",
						room->GetIndex(), info.bUpdated, info.cmdCount, info.processCommandTime, info.updateTime);
					if (info.bUpdated) 
						++updatedRoom;
				}
				Logger::GetInstance().Warn(L"[RoomUpdater::ShowLogicPerformance()] Logic timer over. [{0}/{1}]", totalTime, updatedRoom);
			}
		}
	}
}