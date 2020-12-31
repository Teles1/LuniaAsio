#include <StageServer/Room/RoomManager.h>
#include <Core/Utils/ConfigReader.h>
#include <StageServer/Room/Room.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomManager::RoomManager() {
				m_PoolSize = ConfigInstance().Settings.RoomSize;
			}
			bool RoomManager::RoomJoin(const uint16& roomIndex, UserSharedPtr user, const std::string& roomPass)
			{
				AutoLock lock(mtx);
				if (roomIndex >= m_PoolSize) {
					Logger::GetInstance().Error("RoomJoin Failed: roomIndex over m_PoolSize");
				}

				Room& room = m_Rooms.at(roomIndex);

				return false;
			}

			RoomManager& RoomManagerInstance()
			{
				return RoomManager::GetInstance();
			}
		}
	}
}