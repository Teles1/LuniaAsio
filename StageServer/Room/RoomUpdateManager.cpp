#pragma once
#include <StageServer/Room/RoomUpdateManager.h>
#include <StageServer/Room/Room.h>
#include <StageServer/Room/RoomUpdater.h>
#include <StageServer/User/User.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomUpdateManager::RoomUpdateManager() : m_ThreadCnt(0)
			{
				GetLoggerInstance().Info(L"RoomUpdateManager::RoomUpdateManager()");

				InitUpdater(GetProcessorCnt());
			}


			RoomUpdateManager::RoomUpdateManager(uint16 processorCnt) : m_ThreadCnt(processorCnt)
			{
				GetLoggerInstance().Info(L"RoomUpdateManager::RoomUpdateManager({0})", processorCnt);
				InitUpdater(processorCnt);
			}


			RoomUpdateManager::~RoomUpdateManager()
			{
				DeleteUpdater();

				GetLoggerInstance().Info(L"RoomUpdateManager::~RoomUpdateManager()");
			}


			void RoomUpdateManager::InitUpdater(uint16 processorCnt)
			{
				m_ThreadCnt = processorCnt;

				m_Threads.reserve(m_ThreadCnt);

				for (uint16 i = 0; i < m_ThreadCnt; ++i)
					m_Threads.push_back(std::make_shared<RoomUpdater>(i));
			}


			void RoomUpdateManager::DeleteUpdater()
			{
				for (uint16 i = 0; i < m_ThreadCnt; ++i)
					m_Threads[i]->StopThread();

				m_Threads.clear();
			}


			uint16 RoomUpdateManager::GetProcessorCnt()
			{
				SYSTEM_INFO si;
				GetSystemInfo(&si);
				return si.dwNumberOfProcessors;
			}


			uint16 RoomUpdateManager::GetLeisureThread() // Get smallist roomcnt thread index
			{
				uint16 index = 0;
				uint16 minRoom = m_Threads[0]->GetRoomCnt();

				for (uint16 i = 1; i < m_ThreadCnt; ++i)
					if (m_Threads[i]->GetRoomCnt() < minRoom)
					{
						minRoom = m_Threads[i]->GetRoomCnt();
						index = i;
					}

				return index;
			}


			uint16 RoomUpdateManager::AddRoom(RoomSharedPtr room)
			{
				uint16 index = GetLeisureThread();

				m_Threads[index]->AddRoom(room);

				return index;
			}


			void RoomUpdateManager::DelRoom(RoomSharedPtr room)
			{
				uint16 index = room->GetThreadIndex();

				// spareThreads[ index - m_ThreadCnt ]->DelRoom( room );
				m_Threads[index]->DelRoom(room);
			}


			void RoomUpdateManager::UpdateExpFactor()
			{
				for (auto i = 0; i < m_ThreadCnt; ++i)
					m_Threads[i]->UpdateExpFactor();
			}

			void RoomUpdateManager::NoticeHolidayEvent(uint32 eventId, bool start)
			{
				for (auto i = 0; i < m_ThreadCnt; ++i)
					m_Threads[i]->NoticeHolidayEvent(eventId, start);
			}

			void RoomUpdateManager::Stop()
			{
				for (auto i = 0; i < m_ThreadCnt; ++i)
				{
					m_Threads[i]->Resume();
					m_Threads[i]->StopThread();
				}
			}


			uint16 RoomUpdateManager::GetActiveRoomCnt()
			{
				uint16 activeRoomCnt = 0;

				for (auto i = 0; i < m_ThreadCnt; ++i)
					activeRoomCnt += m_Threads[i]->GetRoomCnt();

				return activeRoomCnt;
			}


			bool RoomUpdateManager::JoinUser(uint16 updaterIndex, RoomSharedPtr room, UserSharedPtr user, const std::string& roomPass)
			{
				return m_Threads[updaterIndex]->JoinUser(room, user, roomPass);
			}


			bool RoomUpdateManager::PartUser(uint16 updaterIndex, RoomSharedPtr room, UserSharedPtr user)
			{
				return m_Threads[updaterIndex]->PartUser(room, user);
			}

} } }