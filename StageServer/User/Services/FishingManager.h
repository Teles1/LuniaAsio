#pragma once
#include <StageServer/Protocol/FromServer.h>
#include <StageServer/Protocol/ToServer.h>
#include <StageServer/Common.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class FishingManager {
				bool m_Fishing;
				uint32 m_BaitHash;
				XRated::ItemPosition m_BaitPos;
				uint16 m_BaitCount;
				float m_Elapsed;
				float m_Duration;
				mutable std::mutex m_FishingMtx;
				Math::PseudoRandomFloat randomFloat;

				enum State { Doing = 0, Wait } state;
				User& m_Owner;
			public:
				FishingManager(User& owner);
				void Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::Start& packet);
				void Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::End& packet);
				void Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::List& packet);
				void Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::Do& packet);
				void Dispatch(const UserSharedPtr, Protocol::ToServer::Fishing::ChangeBaitCount& packet);
				void Init();
				bool Start();
				bool End();
				std::mutex& GetSync();
				float GetRandomFloat();
				Protocol::FromServer::Fishing::Do::Results Do(const ItemPosition& pos, const uint32& hash, const float& dur);
				bool Update(float dt);

				bool IsFishing() const;
				uint8 GetBaitCount() const;
				const XRated::ItemPosition& GetBaitPos() const;
				ItemPosition GetBaitPosToInven() const;
				uint32 GetBaitHash() const;

				void SetBaitCount(const uint16& cnt);
			};
		}
	}
}