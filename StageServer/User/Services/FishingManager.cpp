#include "FishingManager.h"
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			FishingManager::FishingManager() : randomFloat(Math::Random()) {}
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