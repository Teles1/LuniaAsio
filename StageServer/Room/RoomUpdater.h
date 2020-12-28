#ifndef RoomUpdater_H
#define RoomUpdater_H

#pragma once
#include <Core/Runnable.h>
#include <Core/DeltaTimer.h>
#include <set>

namespace Lunia {
	namespace XRated {
		namespace StageServer{
			class User;
			struct RoomUpdater : public Lunia::Runnable {
				typedef std::shared_ptr<User> UserSharedPtr;
			public:
				RoomUpdater(int& index);
				~RoomUpdater();
				void Run();
				void AddRoom(RoomSharedPtr room);
				void DelRoom(RoomSharedPtr room);

				bool JoinUser(RoomSharedPtr room, UserSharedPtr user, const std::string& roomPass);
				bool PartUser(RoomSharedPtr room, UserSharedPtr user);

				void UpdateExpFactor();
				void NoticeHolidayEvent(uint32 eventId, bool start);

				size_t GetRoomCnt();

			protected:
				void UpdateRooms(float dt);
				void ShowLogicPerformance();

			protected:
				std::set<RoomSharedPtr>		m_Rooms;
				DeltaTimer<float>			m_Timer = DeltaTimer<float>(0.0f, FLT_MAX);
				float						m_Dt = 0.0f;
				float						m_PassedTime = 0.0f;

				uint32						m_OldTime = 0;
				uint32						m_NowTime = 0;
				uint32						m_UpdateTime = 0;
				uint32						m_LoopTime = 0;
				uint32						m_UpdateCount = 0;

				int							m_Index;

				std::mutex					m_Mtx;
			};

		}
	}
}
#endif // ! RoomUpdater_H