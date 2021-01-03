#include <Core/Utils/ConfigReader.h>
#include <StageServer/Room/RoomManager.h>
#include <StageServer/Room/Room.h>
#include <StageServer/Room/RoomUpdateManager.h>
#include <Logic/Stage/ScriptLoadThread.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomManager::RoomManager() 
				: m_MaxCnt(0)
				, m_Count(0)
			{
				LoggerInstance().Info("RoomManager::RoomManager()");
				Logic::ScriptLoaderInstance();
				m_PoolSize = ConfigInstance().Get("RoomSize", uint16(150));

				m_Rooms.reserve(m_PoolSize);
				for (uint16 i = 0; i < m_PoolSize; i++)
					m_Rooms.push_back(std::make_shared<Room>(i));

				switch (ConfigInstance().GetKind()) {
				case ServerKind::SquareKind:
					m_RoomKind = Common::ROOMKIND::SQUARE;
					break;
				case ServerKind::PVPKind:
					m_RoomKind = Common::ROOMKIND::PVP;
					break;
				case ServerKind::StageKind:
					m_RoomKind = Common::ROOMKIND::STAGE;
					break;
				default:
					LoggerInstance().Exception("RoomKind not valid");
					break;
				}

				m_RoomUpdateManager = std::make_shared<RoomUpdateManager>();
			}
			bool RoomManager::RoomJoin(const uint16& roomIndex, UserSharedPtr user, const std::string& roomPass)
			{
				AutoLock lock(mtx);
				if (roomIndex >= m_PoolSize) {
					Logger::GetInstance().Error("RoomJoin Failed: roomIndex over m_PoolSize");
				}

				auto &room = m_Rooms.at(roomIndex); // i dont want a copy of the room i just

				auto threadIndex = room->GetThreadIndex();
				if (threadIndex == UINT16_MAX) {
					LoggerInstance().
						Info("Adding room to updaterThread. roomIndex:{0}, threadIndex{1}, userSerial:{2}", roomIndex, threadIndex, user->GetSerial());
					threadIndex = m_RoomUpdateManager->AddRoom( room );
					auto activeRoomCount = m_RoomUpdateManager->GetActiveRoomCnt();
					if (activeRoomCount > m_MaxCnt)
						m_MaxCnt = activeRoomCount;
					++m_Count;
					LoggerInstance().Info("roomIndex:{0}, threadIndex:{1},activeRoomCount:{2} is working by user:{3}",
						roomIndex, threadIndex, activeRoomCount, user->GetSerial());
				}else
					LoggerInstance().Info("Already Active room - roomIndex:{0},threadIndex:{1},activeRoomCount:{2},members:{3}",
					roomIndex, threadIndex, threadIndex, room->UserCount());

				return m_RoomUpdateManager->JoinUser(threadIndex, room, user, roomPass);
			}

			RoomManager& RoomManagerInstance()
			{
				return RoomManager::GetInstance();
			}
		}
	}
}