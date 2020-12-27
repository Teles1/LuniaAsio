#include "Inventory.h"
#include <Info/Info/Database.h>
#include "../Object/Player.h"
#include "../State/States.h"
#include "../LogicDB/LogicDB.h"

using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {

	//const float Inventory::COOLDOWN = 3.0f;

	Inventory::Inventory() : cooldown(0), categoryCooldown(Database::Info::ItemCategory::CATEGORYCNT, 0)
	{
	}

	void Inventory::Init()
	{
		cooldowns.clear();
		std::fill(categoryCooldown.begin(), categoryCooldown.end(), 0.0f);
		cooldown = 0;
		equipments.Init();
	}

	bool Inventory::IsAbleToUse(const Database::Info::ItemInfo* info, Constants::GameTypes gameType, Player* player, const Database::Info::StageInfo* stageInfo)
	{
		if (!info 
			|| (info->Limits.Class != Constants::ClassType::AnyClassType && info->Limits.Class != player->GetClassType())
			|| info->Limits.Level > player->GetLevel() || info->Limits.MaxLevel < player->GetLevel()
			|| info->Limits.PvpLevel > player->GetPvpLevel() || info->Limits.MaxPvpLevel < player->GetPvpLevel() 
			|| info->Limits.WarLevel > player->GetWarLevel() || info->Limits.MaxWarLevel < player->GetWarLevel()
			|| info->Limits.StoredLevel > player->GetStoredLevel() || info->Limits.MaxStoredLevel < player->GetStoredLevel()
			|| info->Limits.RebirthCount > player->GetRebirthCount() || info->Limits.MaxRebirthCount < player->GetRebirthCount())
			return false;

		if(stageInfo != NULL && info->Limits.RebirthCount > stageInfo->RebirthCountEquipLimit)
			return false;

		if ( !(info->Limits.StageType & gameType) ) 
			return false;

		std::vector< std::wstring >::const_iterator cit = info->Limits.StageIds.begin();
		if( stageInfo != NULL )
		{
			bool isUseableStageId = false;
			for( ; cit != info->Limits.StageIds.end(); ++cit )
			{
				if( *cit == stageInfo->Id )
				{
					isUseableStageId = true;											
				}
			}
			if( !info->Limits.StageIds.empty() && isUseableStageId == false )
				return false;

			cit = info->Limits.UnableStageIds.begin();
			for( ; cit != info->Limits.UnableStageIds.end(); ++cit )
			{
				if( *cit == stageInfo->Id )
				{
					return false;
				}
			}
		}

		if ( !info->Limits.BFatal ) {
			if ( player->GetHP() <= 0 )
				return false;
		}

		int Str, Dex, Int, Vit;
		player->GetBaseStat(Str, Dex, Int, Vit);
		if (info->Limits.Str > Str || info->Limits.Dex > Dex || info->Limits.Int >Int || info->Limits.Vit > Vit)
			return false;
		return true;
	}

	uint32 Inventory::UseItem(const Database::Info::ItemInfo* info, Constants::GameTypes gameType, Player* player, const Database::Info::StageInfo* stageInfo)
	{
		if ( cooldown > 0 )
			return Player::ItemUseResult::FailInventoryCoolDownNow;

		if(!IsAbleToUse(info, gameType, player, stageInfo))
			return Player::ItemUseResult::FailNotAbleToUse;

		// cooldown setting.
		switch(info->CooldownCat) {
		case Database::Info::ItemCategory::NOCATEGORY :
			{
				std::list<ItemCooldown>::iterator i = cooldowns.begin(), end = cooldowns.end();
				for ( ; i != end ; ++i ) {
					if ( (*i).id == info->Hash )
						return Player::ItemUseResult::FailItemCoolDownNow;
				}
				cooldowns.push_back(ItemCooldown(info->Hash, info->Cooldown));
			}
			break;
		default :
			if ( categoryCooldown[info->CooldownCat] > 0 ) {
				return Player::ItemUseResult::FailItemCoolDownNow;
			}
			categoryCooldown[info->CooldownCat] = info->Cooldown;
		}

		switch ( info->ItemType ) {
		case Info::ItemInfo::Type::EQUIPMENT :
			if (!equipments.IsEquipped(info->Hash))
				return Player::ItemUseResult::FailNotEquippedEquipment;
			LogicDBInstance().stateDB.AddItemState(info->Hash, player);
			break;
		case Info::ItemInfo::Type::MISC :
		case Info::ItemInfo::Type::INSTANT :
		case Info::ItemInfo::Type::QUEST :
			LogicDBInstance().stateDB.AddItemState(info->Hash, player);
			break;
		case Info::ItemInfo::Type::REAGENT :
			return Player::ItemUseResult::Failure;
		//case Info::ItemInfo::Type::PETITEM :
		//	LogicDBInstance().stateDB.AddItemState(info->Hash, player);
		//	break;
		case Info::ItemInfo::Type::COMBOITEM :
			{
				Database::Info::ActionInfo::StateList::const_iterator i
					= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::AUTOACTION>());
				if (i==info->States.end())
					return Player::ItemUseResult::FailInvalidComboItem;
				/* retrieve */
				std::map<String, String>::const_iterator keyValue=i->params.find(L"Id");
				if (keyValue==i->params.end())
					return Player::ItemUseResult::FailInvalidComboItem;

				player->AutoLinkAction( Database::DatabaseInstance().InfoCollections.Actions.RetriveAutoAction(StringUtil::To<uint32>(keyValue->second.c_str())) );
			}
		default :
			break;
		}

		Database::Info::StateBundleInfo::ConditionList::const_iterator i = info->StateBundleConditions.begin();
		for ( ; i != info->StateBundleConditions.end() ; ++i)
			player->ProcessStateBundle(*i);

		cooldown = INVENTORYCOOLDOWN;

		return Player::ItemUseResult::Success;
	}

	bool Inventory::Equip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, Player* obj, bool bForce, uint16 RebirthCountEquipLimit)
	{
		if ((int)where < 0 || (int)where >= Constants::Equipment::Cnt )
			return false;

		if (info == NULL) {
			return equipments.UnEquip(where, equipmentOld, obj);
		}

		if (!bForce && !equipments.IsAbleToEquip(info, obj, RebirthCountEquipLimit))
			return false;

		if (equipments.GetEquipmentId(where))
			if (!equipments.UnEquip(where, equipmentOld, obj))
				return false;

		equipments.Equip(info, fields, where, obj, RebirthCountEquipLimit);
		return true;
	}
	bool Inventory::Unequip( Constants::Equipment where, uint32& equipmentOld, Player* obj )
	{
		if ((int)where < 0 || (int)where >= Constants::Equipment::Cnt )
			return false;

		equipments.UnEquip(where, equipmentOld, obj);
		return true;
	}

	void Inventory::ValidateEquippedItems(Player* obj, uint16 RebirthCountEquipLimit)
	{
        equipments.ValidateEquippedItems(obj, RebirthCountEquipLimit);
	}

	bool Inventory::IsEnableEquipment(Constants::Equipment where)
	{
        return equipments.IsEnableEquipment(where);
	}

	bool Inventory::ChangeEquipToEquip(Constants::Equipment from, Constants::Equipment to)
	{
		// check in Equip to Equip.
		if(!IsEnableEquipment(from))
		{
            return false;
		}
		return equipments.ChangeEquipToEquip(from,to);
	}

	void Inventory::AddEquipSetBonus(Player* obj)
	{
		equipments.AddEquipSetBonus(obj);
	}

	void Inventory::Update(float dt)
	{
		if ( cooldown > 0 )
			cooldown = cooldown-dt;

		int cnt = Database::Info::ItemCategory::CATEGORYCNT;
		while (cnt--)
			categoryCooldown[cnt] = categoryCooldown[cnt] - ((dt<=0)? 0 : dt);

		std::list<ItemCooldown>::iterator i = cooldowns.begin();
		for ( ; i != cooldowns.end() ; ) {
			ItemCooldown& current = (*i);
			current.time = current.time - dt;
			if ( current.time <= 0 )
				i = cooldowns.erase(i);
			else ++i;
		}
	}

	bool Inventory::PetEquip(const XRated::Pet& pet, const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, Player* obj, bool bForce)
	{
		int pos = ( static_cast< int >( where ) - static_cast< int >( XRated::Constants::Equipment::Pet_Mask ) );
		if ( ( pos < 0 ) || ( pos >= Constants::Equipment::MaxPetEquipCnt ) )
		{
			return false;
		}

		if ( info == NULL ) 
		{
			return equipments.PetUnEquip( where, equipmentOld, obj, pet.PetSerial );
		}

		if ( !bForce && !equipments.IsPetAbleToEquip( info, pet.PetHash ) )
		{
			return false;		
		}

		if ( equipments.GetPetEquipmentId( pet.PetSerial, where ) )
		{
			if ( !equipments.PetUnEquip( where, equipmentOld, obj, pet.PetSerial ) )
			{
				return false;
			}
		}

		equipments.PetEquip( info, fields, where, obj, pet.PetSerial );

		return true;		
	}	


}	}	}
