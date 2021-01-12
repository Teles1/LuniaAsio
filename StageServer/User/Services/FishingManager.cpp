#include "FishingManager.h"
#include <StageServer/User/User.h>
#include <StageServer/User/IUserRoom.h>
#include <Core/Utils/ConfigReader.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			FishingManager::FishingManager(User& owner)
				: randomFloat(Math::Random())
				, m_Owner(owner)
			{}
			void FishingManager::Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::Start& packet)
			{
				const static uint32 maxFishingInStage = 50;

				Protocol::FromServer::Fishing::Start resultPacket;
				resultPacket.PlayerSerial = m_Owner.GetSerial();

				const Database::Info::FishingRodInfo::RodInfo* rodInfo = m_Owner.GetFishingRod();
				if (!rodInfo) {
					resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::FishingRodNeeded;
					m_Owner.Send(resultPacket);
					return;
				}
				if (!m_Owner.IsProperFishingRod(m_Owner.GetFishingRodId())) {
					resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::ProperRodNeeded;
					m_Owner.Send(resultPacket);
					return;
				}
				auto room = m_Owner.GetRoom();
				if (room)
				{
					if (room->GetRoomKind() == Common::SQUARE)
					{
						auto squaresInfo = ConfigInstance().Get<std::vector<Service::SquareInfo>>("SquareInfos");
						if (squaresInfo.size() > room->GetRoomIndex()) {
							if (squaresInfo[m_Owner.GetRoom()->GetRoomIndex()].maxFishing <= room->GetFishingUserCnt()) {
								resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::TooManyFishingUsers;
								m_Owner.Send(resultPacket);
								return;
							}
						}
						else {
							resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::Unknown;
							m_Owner.Send(resultPacket);
							return;
						}
					}
					else
					{
						if (maxFishingInStage <= room->GetFishingUserCnt())
						{
							resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::TooManyFishingUsers;
							m_Owner.Send(resultPacket);
							return;
						}
					}
				}

				if (m_Owner.IsFishingArea() == false) {
					resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::NoFishingArea;
					m_Owner.Send(resultPacket);
					return;
				}

				if (m_Owner.GetBlankSlotCount() < 1) {
					resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::NotEnoughInventory;
					m_Owner.Send(resultPacket);
					return;
				}

				SetBaitCount(rodInfo->NumberOfBait);

				if (Start() == false) {
					resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::AlreadyInFishingMode;
					m_Owner.Send(resultPacket);
					return;
				}
				else {
					resultPacket.Result = Protocol::FromServer::Fishing::Start::Results::Ok;
					room->SendToAll(resultPacket);

					std::pair< uint16, uint16 > flag = room->GetCashItemViewFlag(m_Owner.shared_from_this());
					if ((flag.first & Constants::EquipFlag::CASH_Weapon) != 0)
					{
						m_Owner.ToggleFishingRodVisibility(false);
						flag.first -= 1;
						room->CashItemView(m_Owner.shared_from_this(), flag);
					}
					else
					{
						m_Owner.ToggleFishingRodVisibility(true);
						room->CashItemView(m_Owner.shared_from_this(), flag);
					}

					LoggerInstance().Info("Fishing Started - {}", m_Owner.GetSerial());
				}

				if (!room || IsFishing() == false)
					return;
				room->RequestFishingList(m_Owner.shared_from_this());
				room->AddFishingUser(m_Owner.GetSerial());
			}
			void FishingManager::Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::End& packet)
			{
				Protocol::FromServer::Fishing::End resultPacket;
				if (!End()) {
					resultPacket.Result = Protocol::FromServer::Fishing::End::Results::NotFishingMode;
					m_Owner.Send(resultPacket);
				}
				else {
					resultPacket.Result = Protocol::FromServer::Fishing::End::Results::Ok;
					resultPacket.PlayerSerial = m_Owner.GetSerial();
					auto room = m_Owner.GetRoom();
					room->RemoveFishingUser(m_Owner.GetSerial());
					room->SendToAll(resultPacket);
					std::pair< uint16, uint16 > flag = room->GetCashItemViewFlag(m_Owner.shared_from_this());

					if (!m_Owner.GetFishingRodVisibility() && (flag.first & Constants::EquipFlag::CASH_Weapon) == 0)
					{
						flag.first += 1; // 3.1
						room->CashItemView(m_Owner.shared_from_this(), flag);
					}
					else
						room->CashItemView(m_Owner.shared_from_this(), flag);
				}

			}
			void FishingManager::Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::List& packet)
			{
				auto room = m_Owner.GetRoom();
				if (!room || !IsFishing())
					return;

				room->RequestFishingList(m_Owner.shared_from_this());
			}
			void FishingManager::Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::Do& packet)
			{
				Protocol::FromServer::Fishing::Do resultPacket;
				resultPacket.PlayerSerial = m_Owner.GetSerial();
				if (IsFishing() == false) {
					resultPacket.Result = Protocol::FromServer::Fishing::Do::Results::NotFishingMode;
					m_Owner.Send(resultPacket);
					return;
				}
				const Common::ItemEx* itemInfo = m_Owner.GetItem(packet.BaitPosition);
				if (itemInfo == nullptr || itemInfo->Info == nullptr || itemInfo->Info->Hash != packet.BaitHash || itemInfo->StackedCount == 0) {
					resultPacket.Result = Protocol::FromServer::Fishing::Do::Results::BaitNeeded;
					m_Owner.Send(resultPacket);
					return;
				}
				float duration = Database::DatabaseInstance().InfoCollections.FishigInfos.RetrieveBaitInfo(packet.BaitHash);
				if (duration < 0) {
					resultPacket.Result = Protocol::FromServer::Fishing::Do::Results::NotBait;
					m_Owner.Send(resultPacket);
					return;
				}

				if (m_Owner.GetBlankSlotCount() < 1) {
					resultPacket.Result = Protocol::FromServer::Fishing::Do::Results::NotEnoughInventory;
					m_Owner.Send(resultPacket);
					return;
				}

				resultPacket.Result = Do(packet.BaitPosition, packet.BaitHash, duration);
				if (resultPacket.Result == Protocol::FromServer::Fishing::Do::Results::Ok) {
					m_Owner.GetRoom()->SendToAll(resultPacket);
					/*if (achievement.IsConnected()) {
						Net::Protocol::Achievement::ServerProtocol::FishingCountChanged packet;
						packet.characterName = GetName();
						packet.fishingCount = uint32(1);
						achievement.Send(packet);
					}*/
				}
				else
					m_Owner.Send(resultPacket);
			}
			void FishingManager::Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::ChangeBaitCount& packet)
			{
				Protocol::FromServer::Fishing::ChangeBaitCount resultPacket;
				if (IsFishing() == false) {
					resultPacket.Result = Protocol::FromServer::Fishing::ChangeBaitCount::Results::NotFishingMode;
					m_Owner.Send(resultPacket);
					return;
				}
				else if (packet.Count < 1 || packet.Count > XRated::Fishing::MaxBaitCount) {
					resultPacket.Result = Protocol::FromServer::Fishing::ChangeBaitCount::Results::OutOfRange;
					m_Owner.Send(resultPacket);
					return;
				}
				SetBaitCount(packet.Count);

				resultPacket.Result = Protocol::FromServer::Fishing::ChangeBaitCount::Results::Ok;
				resultPacket.Count = packet.Count;
				m_Owner.Send(resultPacket);
			}
			void FishingManager::Init()
			{
				state = State::Wait; m_Elapsed = 0; m_BaitCount = 1; m_Fishing = false;
			}
			bool FishingManager::Start()
			{
				if (m_Fishing) 
					return false; 
				AutoLock lock(m_FishingMtx); 
				m_Fishing = true; 
				return true;
			}
			bool FishingManager::End()
			{
				if (!m_Fishing) return false;
				AutoLock lock(m_FishingMtx);
				m_Fishing = false;
				state = Wait;
				m_Duration = 0;
				m_Elapsed = 0;
				return true;
			}
			std::mutex& FishingManager::GetSync()
			{
				return m_FishingMtx;
			}
			float FishingManager::GetRandomFloat()
			{
				return randomFloat.Next();
			}
			Protocol::FromServer::Fishing::Do::Results FishingManager::Do(const ItemPosition& pos, const uint32& hash, const float& dur)
			{
				AutoLock lock(m_FishingMtx);
				if (!m_Fishing) 
					return Protocol::FromServer::Fishing::Do::Results::NotFishingMode;
				else if (state != State::Wait) 
					return Protocol::FromServer::Fishing::Do::Results::AlreadyFishingNow;
				state = State::Doing;
				m_Elapsed = 0;
				m_BaitHash = hash;
				m_BaitPos = pos;
				m_Duration = dur;
				return Protocol::FromServer::Fishing::Do::Results::Ok;
			}
			bool FishingManager::Update(float dt)
			{ //return true when fishing time over.
				if (!m_Fishing) return false;
				switch (state) {
				case State::Doing:
					m_Elapsed += dt;
					if (m_Elapsed >= m_Duration) {
						m_Elapsed = 0;
						state = State::Wait;
						return true;
					}
					break;
				}
				return false;
			}
			bool FishingManager::IsFishing() const
			{
				return m_Fishing;
			}
			uint8 FishingManager::GetBaitCount() const
			{
				return m_BaitCount;
			}
			const XRated::ItemPosition& FishingManager::GetBaitPos() const
			{
				return m_BaitPos;
			}
			ItemPosition FishingManager::GetBaitPosToInven() const
			{
				ItemPosition pos = m_BaitPos; 
				pos.Bag -= 1; 
				return pos;
			}
			uint32 FishingManager::GetBaitHash() const
			{
				return m_BaitHash;
			}
			void FishingManager::SetBaitCount(const uint16& cnt)
			{
				m_BaitCount = cnt;
			}
		}
	}
}