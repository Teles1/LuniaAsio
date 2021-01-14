#include <Core/Utils/ConfigReader.h>
#include <StageServer/Room/RoomManager.h>
#include <StageServer/Room/Room.h>
#include <StageServer/Room/RoomUpdateManager.h>
#include <Logic/Stage/ScriptLoadThread.h>
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomManager::RoomManager() 
				: totalCnt(0)
				, maxCnt(0)
				, poolSize(0)
			{
				LoggerInstance().Info("RoomManager::RoomManager()");
				Logic::ScriptLoaderInstance();
				poolSize = ConfigInstance().Get("RoomSize", uint16(150));

				rooms.reserve(poolSize);
				for (uint16 i = 0; i < poolSize; i++)
					rooms.push_back(std::make_shared<Room>(i));

				switch (ConfigInstance().GetKind()) {
				case ServerKind::SquareKind:
					roomKind = Common::ROOMKIND::SQUARE;
					break;
				case ServerKind::PVPKind:
					roomKind = Common::ROOMKIND::PVP;
					break;
				case ServerKind::StageKind:
					roomKind = Common::ROOMKIND::STAGE;
					break;
				default:
					LoggerInstance().Exception("RoomKind not valid");
					break;
				}

				roomUpdateMgr = std::make_shared<RoomUpdateManager>();
			}
			
			RoomManager::~RoomManager()
			{
				Logic::ScriptLoaderInstance().StopThread();
				ReleaseAllRooms();
				LoggerInstance().Warn(L"RoomManager::~RoomManager() - maxCnt({}), totalCnt({})", maxCnt, totalCnt);
			}
			
			void RoomManager::ReleaseAllRooms()
			{
				AutoLock lock(cs);

				for (size_t index = 0; index < poolSize; ++index)
					rooms[index].reset();
				rooms.clear();
			}


			bool RoomManager::RoomJoin(uint16 roomIndex, UserSharedPtr user, const std::string& roomPass)
			{
				AutoLock lock(cs);

				if (roomIndex >= poolSize) {
					LoggerInstance().Error(L"RoomJoin Failed :room index over roomindex[{}]", roomIndex);
					return false;
				}

				auto& room = rooms.at(roomIndex);

				int threadIndex = room->GetThreadIndex();
				if (threadIndex < 0)
				{
					LoggerInstance().Info((L"Add room to updater thread. (roomIndex:{}, threadIndex:{}, {})", roomIndex, threadIndex, user->GetName().c_str()));
					threadIndex = roomUpdateMgr->AddRoom(room);

					size_t activeRoomCnt = roomUpdateMgr->GetActiveRoomCnt();
					if (activeRoomCnt > maxCnt)
						maxCnt = activeRoomCnt;

					++totalCnt;

					//OutputInstance().Info_RoomCreated(roomIndex, room->GetRoomKind(), activeRoomCnt, maxCnt);
					LoggerInstance().Warn(L"a roomCs(roomindex:{}, threadindex:{}, activeRoomCnt:{}) is working by {}", roomIndex, threadIndex, activeRoomCnt, user->GetName().c_str());
				}
				else
				{
					LoggerInstance().Info(L"Already activated room. (roomIndex:{}, threadIndex:{}, Members:{} )", roomIndex, threadIndex, room->UserCount());
				}

				return roomUpdateMgr->JoinUser(threadIndex, room, user, roomPass);
			}


			/* this method called by UserManager::DBTerminated and UserManager::DBStageMoved */
			bool RoomManager::RoomOut(uint16 roomIndex, UserSharedPtr user)
			{
				AutoLock lock(cs);

				if (roomIndex >= poolSize)
					return false;

				auto& room = rooms.at(roomIndex);

				int threadIndex = room->GetThreadIndex();
				if (threadIndex < 0)
				{
					LoggerInstance().Error(L"the roomCs({}) is already out of thread", room->GetRoomIndex());
					return false;
				}

				if (!roomUpdateMgr->PartUser(threadIndex, room, user))
				{
					LoggerInstance().Error(L"a user({}) is failed to part away from the roomCs(0x%p) on threadindex {}", user->GetName().c_str(), room->GetRoomIndex(), threadIndex);
					return false;
				}

				if (room->UserCount() == 0)
				{
					LoggerInstance().Warn(L"a roomCs(roomindex:{}, threadindex:{}) went empty by {}", roomIndex, threadIndex, user->GetName().c_str());
					roomUpdateMgr->DelRoom(room);
					//OutputInstance().Info_RoomReleased(roomIndex, room->GetRoomKind(), NowCnt(), maxCnt);
					room->Clear();
				}

				return true;
			}

			bool RoomManager::ClearRoom(uint16 roomIndex, UserSharedPtr user)
			{
				AutoLock lock(cs);

				if (roomIndex >= poolSize)
					return false;

				auto& room = rooms.at(roomIndex);

				if (room->UserCount() == 0)
				{
					//LoggerInstance().Info(( L"a roomCs(0x%p, roomindex:{}, threadindex:{}) went empty by {}", &(room->GetSyncRoom()), roomIndex, threadIndex, user->GetName().c_str() ));
					int threadIndex = room->GetThreadIndex();
					if (threadIndex >= 0)
					{
						roomUpdateMgr->DelRoom(room);
					}
					//OutputInstance().Info_RoomReleased(roomIndex, room->GetRoomKind(), NowCnt(), maxCnt);
					room->Clear();
				}

				return true;
			}

			void RoomManager::UpdateExpFactor()
			{
				roomUpdateMgr->UpdateExpFactor();
			}

			void RoomManager::NoticeHolidayEvent(uint32 eventId, bool start)
			{
				roomUpdateMgr->NoticeHolidayEvent(eventId, start);
			}

			bool RoomManager::IsRoomMissionEnded(const uint32 index) const
			{
				auto& room = rooms.at(index);
				return room->IsNowCampfire();
			}

			bool RoomManager::IsRoomAvailable(const uint32 index, const uint16 id) const
			{
				auto& room = rooms.at(index);

				if (room->GetRoomID() == id)
					return true;
				else
					return false;
			}

			bool RoomManager::IsUserExist(const std::wstring& userName, const uint32 roomIndex) const
			{
				auto& room = rooms.at(roomIndex);
				return room->IsUserExist(userName);
			}

			int RoomManager::GetUserCnt(uint16 roomIndex)
			{
				if (roomIndex >= poolSize)
					return 0;

				return rooms.at(roomIndex)->UserCount();
			}


			int RoomManager::NowCnt()
			{
				return roomUpdateMgr->GetActiveRoomCnt();
			}


			void RoomManager::RoomsInfo()
			{
				// [0]2384701928374:1 - 1/4
				printf("\n ---- Room List ----\n");

				RoomSharedPtr room;
				for (size_t i = 0; i < poolSize; ++i)
				{
					room = rooms.at(i);

					if (room->GetThreadIndex() < 0)
						continue;

					printf("%2d %8d/%2d {}", room->GetIndex(), room->GetCurrentStage().StageGroupHash, room->GetCurrentStage().Level, room->UserCount());

					if (((i + 1) % 4) == 0)
						printf("\n");
					else
						printf(" ");
				}
				printf("\n");
			}

			void RoomManager::Stop()
			{
				roomUpdateMgr->Stop();
			}

			void RoomManager::ShowCashItemViewInfo()
			{
				printf("\n ---- CashItemViewInfo ----\n");

				RoomSharedPtr room;
				for (size_t i = 0; i < poolSize; ++i)
				{
					room = rooms.at(i);

					if (room->GetThreadIndex() < 0)
						continue;

					printf("%2d : %8d", room->GetIndex(), room->GetCashItemViewCnt());

					if (((i + 1) % 4) == 0)
						printf("\n");
					else
						printf(" ");
				}
				printf("\n");
			}

			void RoomManager::ClearCashItemViewInfo(int roomIndex)
			{
				if (roomIndex >= 0 && roomIndex < poolSize) {
					RoomSharedPtr room = rooms.at(roomIndex);
					if (room) {
						room->ClearCashItemViewList();
						printf("\n room:{} CashItemViewInfo Cleared.\n", room->GetIndex());
					}
				}
			}
			
			RoomManager& RoomManagerInstance()
			{
				return RoomManager::GetInstance();
			}
		}
	}
}