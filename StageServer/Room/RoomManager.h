#ifndef RoomManager_H
#define RoomManager_H
#pragma once
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			class Room;
			typedef std::shared_ptr<User> UserSharedPtr;
			struct RoomManager {
			public:
				bool RoomJoin(const uint16& roomIndex, UserSharedPtr user, const std::string& roomPass);
			private:
				uint16							m_PoolSize;
				std::mutex						mtx;
				std::vector<Room>				m_Rooms;
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
			RoomManager& RoomManagerInstance();
		}
	}
}
#endif // ! RoomManager_H