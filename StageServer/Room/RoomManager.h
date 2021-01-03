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
			struct RoomManager {
			public:
				bool RoomJoin(const uint16& roomIndex, UserSharedPtr user, const std::string& roomPass);
			private:
				uint16							m_PoolSize;
				uint16							m_MaxCnt;
				uint16							m_Count;
				std::mutex						mtx;
				std::vector<RoomSharedPtr>		m_Rooms;
				Common::ROOMKIND				m_RoomKind;
				RoomUpdateManagerSharedPtr		m_RoomUpdateManager;
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
#pragma endregion Singleton
			};
			typedef std::shared_ptr<RoomManager> RoomManagerSharedPtr;
			RoomManager& RoomManagerInstance();
		}
	}
}
#endif // ! RoomManager_H