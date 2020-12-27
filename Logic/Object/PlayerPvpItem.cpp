#include "PlayerPvpItem.h"
#include "../LogicDB/LogicDB.h"

namespace Lunia { namespace XRated {	namespace Logic {


	void PlayerPvpItem::Use()
	{
		// State들이 있다면 사용한 player에게 적용해주자.
		LogicDBInstance().stateDB.AddItemState(pvpItemInfo->Hash, this);

		// StateBudle이 있다면 조건에 따라 대상에 bundle을 적용해주자.
		Database::Info::StateBundleInfo::ConditionList::iterator i = pvpItemInfo->StateBundleConditions.begin();
		for ( ; i != pvpItemInfo->StateBundleConditions.end() ; ++i)
			ProcessStateBundle(*i);

		itemCooldown = pvpItemInfo->Cooldown;
		// Projectile이 있다면 projectile을 생성해주자.

	}

	bool PlayerPvpItem::UsePvpItem()
	{
		if ( IsUnableToUsePvpItem() ) //스턴이거나 공중에 뜨거나 넘어진 상태라면 아이템을 발동 할 수 없다.
			return false;

		if ( itemCooldown > 0 ) { // 쿨다운으로 아직 아이템을 사용 할 수 없다.
#ifdef _SINGLE
		Lunia_IMPORTANT((L"[PlayerPvpItem::UsePvpItem] Unable to item because of cooldown."));
#endif
			return false;
		}
		Use(); //사용해부러

		stageData->UsePvpItem(this, pvpItemInfo->Hash);

		if ( --useLeft == 0 ) { //사용횟수가 더이상 남아 있지 않다.
			SetState(ItemState::None);
			pvpItemInfo = NULL;
		}
		return true;
	}

	bool PlayerPvpItem::ActivatePvpItem()
	{
		if ( IsUnableToUsePvpItem() ) //스턴이거나 공중에 뜨거나 넘어진 상태라면 아이템을 발동 할 수 없다.
			return false;

		if (!(pvpItemInfo && useLeft)) throw; //아이템 정보가 존재하고 사용가능 횟수도 남아 있다.

		Database::Info::ItemInfo* tempInfo = pvpItemInfo; // 한개 짜리 아이템이면 사용되면서 iteminfo가 없어지므로 임시로 저장하자.
		if ( useLeft != 1 ) { //사용횟수가 존재 하고 한번 이상 쓸수 있는 아이템이다.
			SetState(ItemState::ActivateItem);
			stageData->UsePvpItem(this, tempInfo->Hash);
		}

		UsePvpItem(); //ActivateItem 상태로 넘어가지 않고 바로 발동되어 버린다. 발동후엔 다시 습득가능 상태로 변경된다.

		return true;
	}

	bool PlayerPvpItem::DestroyPvpItem()
	{
		if ( IsUnableToUsePvpItem() ) //스턴이거나 공중에 뜨거나 넘어진 상태라면 아이템을 발동 할 수 없다.
			return false;
		SetState(ItemState::None);
		pvpItemInfo = NULL;
		stageData->DropPvpItem(this);
		return true;
	}

	void PlayerPvpItem::Die()
	{
		SetState(ItemState::None);
		pvpItemInfo = NULL;
		useLeft = 0;
		itemCooldown = 0;

		Player::Die();
	}


	void PlayerPvpItem::Initialize(IGameStateDatabase* db, const std::wstring& name, uint16 lv, uint32 xp, uint32 money, uint32 bankMoney,
						uint16 storedLevel, uint16 rebirthCount, uint32 ladderPoint, uint16 ladderMatchCount, uint32 ladderWinCount, uint32 ladderLoseCount, uint32 achievementScore, uint16 storedSkillPoint,
						uint16 life, uint16 skillpoint, uint16 addedSkillpointPlus, uint8 team, float expFactor, float hpFactor, float mpFactor, bool isBalanced,
						std::vector<Pet>& pets, std::vector<PetCaredBySchool>& petCared, const Lunia::DateTime& lastRebirthDateTime)
	{

		Player::Initialize(db, name, lv, xp, money, bankMoney, storedLevel, rebirthCount, storedSkillPoint
						, ladderPoint, ladderMatchCount, ladderWinCount, ladderLoseCount, achievementScore, life, 0, skillpoint, addedSkillpointPlus
						, team, expFactor, hpFactor, mpFactor, isBalanced, pets, petCared, lastRebirthDateTime);
		SetState(ItemState::None);
		pvpItemInfo = NULL;
	}

	bool PlayerPvpItem::AcquireItem(Database::Info::ItemInfo* info)
	{
		switch ( itemState ) {
		case ItemState::None :
			// 일단 아이템을 먹을 수 있는 상태인지를 검사해야 한다.
			//if( stateBundleManager.GetStateBundle(Database::Info::StateBundleInfo::Sorts::PvpItem) ) // item statebundle buffer를 검사.
			//	return false; //아직 아이템의 효력이 작동중이다. 새로운 아이템을 먹을 수 없다.

			pvpItemInfo = info;
			SetState(ItemState::HavingItem);
			useLeft = info->MaxUse;
			itemCooldown = 0;
			break;
		case ItemState::HavingItem :
		case ItemState::ActivateItem :
			return false;
		default :
			throw;
		}
		return true;
	}

	bool PlayerPvpItem::Command(Action::Action::CommandType state, uint32 param)
	{
		//이건 사실 바보스런 처리 방식이다. 기존에 액션 시스템에 완전히 위배되는 삽질이건만.. 쉽고 편한쪽으로 갈라고 이따구 짓거릴 하고 있다.

		switch ( itemState ) {
		case ItemState::None :
			break;
		case ItemState::HavingItem : // 아이템을 발동할 수 있으면 발동, 파기 할수 있으면 파기
			switch( state ) {
			//case Action::Action::CommandType::ATTACKA :
			case Action::Action::CommandType::DOWNATTACK :
				return ActivatePvpItem();
			//case Action::Action::CommandType::ATTACKB :
			//	return DestroyPvpItem();
			//case Action::Action::CommandType::ATTACKARELEASE :
			//case Action::Action::CommandType::ATTACKBRELEASE :
			//	return false;
			}
			break;
		case ItemState::ActivateItem :
			switch ( state ) {
			case Action::Action::CommandType::DOWNATTACK :
			//case Action::Action::CommandType::ATTACKA :
				return UsePvpItem();
			//case Action::Action::CommandType::ATTACKARELEASE :
			//case Action::Action::CommandType::ATTACKB :
			//case Action::Action::CommandType::ATTACKBRELEASE :
			//	return false;
			}
			break;
		default :
			throw;
		}

		return Player::Command(state, param);
	}

}	}	}


