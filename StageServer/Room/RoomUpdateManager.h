#ifndef RoomUpdateManager_H
#define RoomUpdateManager_H
#pragma once
namespace Lunia {
	namespace XRated {
			namespace StageServer {
				class User;
				class Room;
				class RoomUpdater;
				typedef std::shared_ptr<User> UserSharedPtr;
				typedef std::shared_ptr<Room> RoomSharedPtr;
				typedef std::shared_ptr<RoomUpdater> RoomUpdaterSharedPtr;
				class RoomUpdateManager
				{
				public:
					RoomUpdateManager();
					RoomUpdateManager(uint16 processorCnt);
					~RoomUpdateManager();

					uint16 AddRoom(RoomSharedPtr room); // major method
					void DelRoom(RoomSharedPtr room); // major method

					// TODO : AddHeavyRoom(Room* room);

					void UpdateExpFactor();
					void NoticeHolidayEvent(uint32 eventId, bool start);

					void Stop();

					uint16 GetActiveRoomCnt();

				public:
					bool JoinUser(uint16 updaterIndex, RoomSharedPtr room, UserSharedPtr user, const std::string& roomPass);
					bool PartUser(uint16 updaterIndex, RoomSharedPtr room, UserSharedPtr user);

					uint16 GetProcessorCnt();
				protected:
					void InitUpdater(uint16 processorCnt);
					void DeleteUpdater();

					uint16 GetLeisureThread(); // most important method

				protected:
					uint16 m_ThreadCnt;
					std::vector< RoomUpdaterSharedPtr > m_Threads; // for normal rooms
					//std::vector< RoomUpdater* > spareThreads; // for heavy rooms
				};
				typedef std::shared_ptr<RoomUpdateManager> RoomUpdateManagerSharedPtr;
			}
	}
}
#endif // !RoomUpdateManager_H