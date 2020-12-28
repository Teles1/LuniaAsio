#include <StageServer/Room/RoomManager.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomManager& GetRoomManager()
			{
				return RoomManager::GetInstance();
			}
		}
	}
}