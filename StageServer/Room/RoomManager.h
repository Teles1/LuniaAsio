#pragma once
#include <StageServer/Room/Room.h>
#include <Core/Utils/ConfigReader.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			struct RoomManager {
			
			private:
				std::vector<Room> m_Rooms;
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
				inline RoomManager() {}
			};
			RoomManager& GetRoomManager();
		}
	}
}