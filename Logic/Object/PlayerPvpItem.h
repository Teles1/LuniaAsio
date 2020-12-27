#pragma once
#include "Player.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class PlayerPvpItem : public Player {
		enum ItemState {
			None=0,			// Doesn't have any item
			HavingItem,		// Acquired item but not activate. can active item or drop item
			ActivateItem,	// Activate a item, cannot drop item and can use item.
		};

		ItemState itemState; ///< player state about pvp item.
		Database::Info::ItemInfo* pvpItemInfo; ///< pvp item info
		uint16 useLeft; ///< usable count
		float itemCooldown; ///< 여러발 사용하는 아이템의 쿨다운을 표시. 다른 아이템이라면 상관없다.

	protected :
		inline bool IsUnableToUsePvpItem() { return CheckState(Database::Info::StateInfo::FLIED); }
		void SetState(ItemState state) {
#ifdef _SINGLE
			Lunia_IMPORTANT(( L"[PlayerPvpItem::SetState] state change to [{}]", state ));
			//stageData->NPCDebugMessage(GetSerial(), L"IdleNoneUpdate()");
#endif
			itemState = state;
		}


		void Use();
        /* Use activated item. **/
		bool UsePvpItem();
		/* Activate acquired item. Change to ActivateItem mode **/
		bool ActivatePvpItem();
		/* Drop acquired item and change to None mode to acquire new items. **/
		bool DestroyPvpItem();

	public :
		PlayerPvpItem(Constants::ClassType type) : Player(type) {}
		void Die();
		bool IsPlayerPvpItem() const { return true; }
		bool AcquireItem(Database::Info::ItemInfo* info);


		void Initialize(IGameStateDatabase* db, const std::wstring& name, uint16 lv, uint32 xp, uint32 money, uint32 bankMoney,
							uint16 storedLevel, uint16 rebirthCount, uint32 ladderPoint, uint16 ladderMatchCount, uint32 ladderWinCount, uint32 ladderLoseCount, uint32 achievementScore, uint16 storedSkillPoint,
							uint16 life, uint16 skillpoint, uint16 addedSkillpointPlus, uint8 team, float expFactor, float hpFactor, float mpFactor, bool isBalanced,
							std::vector<XRated::Pet>& pets, std::vector<PetCaredBySchool>& petCared, const Lunia::DateTime& lastRebirthDateTime);
		bool Update(float dt, IGameStateDatabase* db) { itemCooldown -= dt; return Player::Update(dt, db); }
		bool Command(Action::Action::CommandType state, uint32 param);
	};

}	}	}
