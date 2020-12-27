#include "PlayerPvpItem.h"
#include "../LogicDB/LogicDB.h"

namespace Lunia { namespace XRated {	namespace Logic {


	void PlayerPvpItem::Use()
	{
		// State���� �ִٸ� ����� player���� ����������.
		LogicDBInstance().stateDB.AddItemState(pvpItemInfo->Hash, this);

		// StateBudle�� �ִٸ� ���ǿ� ���� ��� bundle�� ����������.
		Database::Info::StateBundleInfo::ConditionList::iterator i = pvpItemInfo->StateBundleConditions.begin();
		for ( ; i != pvpItemInfo->StateBundleConditions.end() ; ++i)
			ProcessStateBundle(*i);

		itemCooldown = pvpItemInfo->Cooldown;
		// Projectile�� �ִٸ� projectile�� ����������.

	}

	bool PlayerPvpItem::UsePvpItem()
	{
		if ( IsUnableToUsePvpItem() ) //�����̰ų� ���߿� �߰ų� �Ѿ��� ���¶�� �������� �ߵ� �� �� ����.
			return false;

		if ( itemCooldown > 0 ) { // ��ٿ����� ���� �������� ��� �� �� ����.
#ifdef _SINGLE
		Lunia_IMPORTANT((L"[PlayerPvpItem::UsePvpItem] Unable to item because of cooldown."));
#endif
			return false;
		}
		Use(); //����غη�

		stageData->UsePvpItem(this, pvpItemInfo->Hash);

		if ( --useLeft == 0 ) { //���Ƚ���� ���̻� ���� ���� �ʴ�.
			SetState(ItemState::None);
			pvpItemInfo = NULL;
		}
		return true;
	}

	bool PlayerPvpItem::ActivatePvpItem()
	{
		if ( IsUnableToUsePvpItem() ) //�����̰ų� ���߿� �߰ų� �Ѿ��� ���¶�� �������� �ߵ� �� �� ����.
			return false;

		if (!(pvpItemInfo && useLeft)) throw; //������ ������ �����ϰ� ��밡�� Ƚ���� ���� �ִ�.

		Database::Info::ItemInfo* tempInfo = pvpItemInfo; // �Ѱ� ¥�� �������̸� ���Ǹ鼭 iteminfo�� �������Ƿ� �ӽ÷� ��������.
		if ( useLeft != 1 ) { //���Ƚ���� ���� �ϰ� �ѹ� �̻� ���� �ִ� �������̴�.
			SetState(ItemState::ActivateItem);
			stageData->UsePvpItem(this, tempInfo->Hash);
		}

		UsePvpItem(); //ActivateItem ���·� �Ѿ�� �ʰ� �ٷ� �ߵ��Ǿ� ������. �ߵ��Ŀ� �ٽ� ���氡�� ���·� ����ȴ�.

		return true;
	}

	bool PlayerPvpItem::DestroyPvpItem()
	{
		if ( IsUnableToUsePvpItem() ) //�����̰ų� ���߿� �߰ų� �Ѿ��� ���¶�� �������� �ߵ� �� �� ����.
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
			// �ϴ� �������� ���� �� �ִ� ���������� �˻��ؾ� �Ѵ�.
			//if( stateBundleManager.GetStateBundle(Database::Info::StateBundleInfo::Sorts::PvpItem) ) // item statebundle buffer�� �˻�.
			//	return false; //���� �������� ȿ���� �۵����̴�. ���ο� �������� ���� �� ����.

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
		//�̰� ��� �ٺ����� ó�� ����̴�. ������ �׼� �ý��ۿ� ������ ����Ǵ� �����̰Ǹ�.. ���� ���������� ����� �̵��� ���Ÿ� �ϰ� �ִ�.

		switch ( itemState ) {
		case ItemState::None :
			break;
		case ItemState::HavingItem : // �������� �ߵ��� �� ������ �ߵ�, �ı� �Ҽ� ������ �ı�
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


