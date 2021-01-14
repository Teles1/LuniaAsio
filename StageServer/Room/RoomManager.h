#ifndef RoomManager_H
#define RoomManager_H
#include <StageServer/Common.h>
#pragma once
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			class Room;
			class RoomUpdateManager;

			typedef std::shared_ptr<User>				UserSharedPtr;
			typedef std::shared_ptr<Room>				RoomSharedPtr;
			typedef std::shared_ptr<RoomUpdateManager>	RoomUpdateManagerSharedPtr;
			typedef std::map<uint16, RoomSharedPtr> roomMap;

			struct RoomManager {
				bool RoomJoin(uint16 roomIndex, UserSharedPtr user, const std::string& roomPass);
				bool RoomOut(uint16 roomIndex, UserSharedPtr user);
				bool ClearRoom(uint16 roomIndex, UserSharedPtr user);

				int GetUserCnt(uint16 roomIndex);

				//void Update(float dt);
				void Stop();

				int TotalCnt() { return totalCnt; }
				int MaxCnt() { return maxCnt; }
				int NowCnt();
				int PoolSize() { return poolSize; }

				void SetRoomKind(Common::ROOMKIND kind) { roomKind = kind; }
				Common::ROOMKIND RoomKind() { return roomKind; }

				void RoomsInfo(); // PrintRoomList
				const roomMap& GetActiveRooms();
				void UpdateExpFactor();
				void NoticeHolidayEvent(uint32 eventId, bool start);

				bool IsRoomMissionEnded(const uint32 index) const;
				bool IsRoomAvailable(const uint32 index, const uint16 id) const;
				bool IsUserExist(const std::wstring& userName, const uint32 roomIndex) const;

				void ShowCashItemViewInfo();
				void ClearCashItemViewInfo(int roomIndex);

			private:
				void ReleaseAllRooms();
				void ServerInfoLog(float dt);

			private:
				std::vector< RoomSharedPtr > rooms;
				RoomUpdateManagerSharedPtr roomUpdateMgr;
				size_t totalCnt;
				size_t maxCnt;
				size_t poolSize;

				Common::ROOMKIND		roomKind;

				std::mutex cs; // <- for activeUserMap (when insert and remove and loop)
#pragma region Singleton
			public://Singleton
				RoomManager(const RoomManager&) = delete; //anti creation  of a copy
				RoomManager& operator= (const RoomManager&) = delete; // anti copy
				RoomManager(RoomManager&&) = delete;
				RoomManager& operator=(RoomManager&&) = delete;
				inline static RoomManager& GetInstance() {
					static RoomManager								m_RoomManagerInstance;
					return m_RoomManagerInstance;
				}
			private:
				RoomManager();
				~RoomManager();
#pragma endregion Singleton
			};
			typedef std::shared_ptr<RoomManager> RoomManagerSharedPtr;
			RoomManager& RoomManagerInstance();
		}
	}
}
#endif // ! RoomManager_H