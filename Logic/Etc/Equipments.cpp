#include "Equipments.h"
#include "../Object/Player.h"
#include "../State/States.h"
#include "../LogicDB/LogicDB.h"
#include <Info/Info/Items/Item.h>

using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {


	Equipments::Equipments()
	{
		for ( int a=0; a<XRated::Constants::Equipment::Cnt ; ++a) {
			parts[a].name = 0;
			parts[a].enable = true;
			parts[a].states.clear();
			parts[a].instance = 0;
			parts[a].levelConditions.clear();
		}
	}

	Equipments::~Equipments()
	{
	}

	void Equipments::Init()
	{
		for ( int a=0; a<XRated::Constants::Equipment::Cnt ; ++a) {
			parts[a].name = 0;
			parts[a].enable = true;
			parts[a].states.clear();
			parts[a].instance = 0;
			parts[a].levelConditions.clear();
		}

		petsParts.clear();
	}

	bool Equipments::IsEquipped(uint32 name) const
	{
		for (int a=0;a<XRated::Constants::Equipment::Cnt;++a) {
			if (parts[a].name == name)
				return true;
		}
		return false;
	}

	bool Equipments::IsAbleToEquip(const Database::Info::ItemInfo* info, Player* obj, uint16 RebirthCountEquipLimit) const
	{
		if (!info
			|| info->ItemType != Database::Info::ItemInfo::Type::EQUIPMENT
			|| (info->Limits.Class != Constants::ClassType::AnyClassType && info->Limits.Class != obj->GetClassType())
			|| info->Limits.Level > obj->GetLevel()
			|| info->Limits.PvpLevel > obj->GetPvpLevel()
			|| info->Limits.WarLevel > obj->GetWarLevel()
			|| info->Limits.StoredLevel > obj->GetStoredLevel()
			|| info->Limits.RebirthCount > obj->GetRebirthCount())
			return false;
	
		if(info->Limits.RebirthCount > RebirthCountEquipLimit)
			return false;

		int Str, Dex, Int, Vit;
		obj->GetBaseStat(Str, Dex, Int, Vit);
		if (info->Limits.Str > Str || info->Limits.Dex > Dex || info->Limits.Int >Int || info->Limits.Vit > Vit)
			return false;
		return true;
	}

	uint32 Equipments::GetEquipmentId(Lunia::XRated::Constants::Equipment part, bool isCheckEnable) const
	{
		if(isCheckEnable == true && parts[(int)part].enable == false)
			return 0;

        return parts[(int)part].name;
	}

	void Equipments::Equip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, Player* obj, uint16 RebirthCountEquipLimit)
	{
		parts[(int)where].name = info->Hash;
		parts[(int)where].instance = fields;

		if ( !IsAbleToEquip(info, obj, RebirthCountEquipLimit) ) 
		{
			parts[(int)where].enable = false;
			return;
		}
		parts[(int)where].enable = true;
		LogicDBInstance().stateDB.AddItemState(info->Hash, obj, parts[where].states);
		AddEnchantState(info, fields, where, obj, RebirthCountEquipLimit);
	}

	void Equipments::AddEnchantState(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, Player* obj, uint16 rebirthCountEquipLimit )
	{
		//Enchant
		//Database::Enchant::EnchantBitfields

		if ( Enchant::IsEnchantable(info) ) {
			std::vector<Database::Info::ActionInfo::State> states;
			Enchant::GetItemInstanceForServer(info, fields, states);
			std::vector<Database::Info::ActionInfo::State>::iterator i = states.begin();
			std::vector<Database::Info::ActionInfo::State>::iterator iEnd = states.end(); 
			for ( ; i != iEnd ; ++i ) {
				Stat::IState* state = Stat::CreateState((*i).type, obj, &(*i).params, &(*i).optionalStates);
				obj->AddState(state);
				parts[where].states.push_back(state);
			}
		} else {
			Enchant::NormalBitfields normal(fields);
			std::pair< Enchant::CashEnchantInfo*, Enchant::CashEnchantInfo* > info = DatabaseInstance().InfoCollections.EnchantTables.GetCashEnchantInfo(normal.CashEnchant1, normal.CashEnchant2);
			if (info.first ) 
			{
				if(DatabaseInstance().InfoCollections.EnchantTables.IsCashEnchantAvailable(info.first, obj->GetLevel(), obj->GetStoredLevel(), obj->GetRebirthCount(), rebirthCountEquipLimit))
				{
					std::vector<Database::Info::ActionInfo::State>::iterator i = info.first->States.begin();
					std::vector<Database::Info::ActionInfo::State>::iterator iEnd = info.first->States.end(); 
					for ( ; i != iEnd ; ++i ) 
					{
						Stat::IState* state = Stat::CreateState((*i).type, obj, &(*i).params, &(*i).optionalStates);
						obj->AddState(state);
						parts[where].states.push_back(state);
					}
				}
				else
				{
					parts[where].levelConditions.push_back( obj->GetLevel() );
					std::sort( parts[where].levelConditions.begin(), parts[where].levelConditions.end() );
				}
			}
			if (info.second )
			{
				if( DatabaseInstance().InfoCollections.EnchantTables.IsCashEnchantAvailable(info.second, obj->GetLevel(), obj->GetStoredLevel(), obj->GetRebirthCount(),rebirthCountEquipLimit)  )
				{
					std::vector<Database::Info::ActionInfo::State>::iterator i = info.second->States.begin();
					std::vector<Database::Info::ActionInfo::State>::iterator iEnd = info.second->States.end(); 
					for ( ; i != iEnd ; ++i ) 
					{
						Stat::IState* state = Stat::CreateState((*i).type, obj, &(*i).params, &(*i).optionalStates);
						obj->AddState(state);
						parts[where].states.push_back(state);
					}
				}
				else
				{
					parts[where].levelConditions.push_back( obj->GetLevel() );
					std::sort( parts[where].levelConditions.begin(), parts[where].levelConditions.end() );
				}
			}
		}
	}

	bool Equipments::IsEnableEquipment(Constants::Equipment where)
	{
        if(parts[where].name == 0)
		{
			return false;
		}
        
		return parts[where].enable;
	}

	void Equipments::ValidateEquippedItems(Player* obj, uint16 RebirthCountEquipLimit)
	{
		for ( int a=0; a<XRated::Constants::Equipment::Cnt ; ++a) 
		{
			if (parts[a].name == 0) continue;

			const Database::Info::ItemInfo* info=DatabaseInstance().InfoCollections.Items.Retrieve(parts[a].name);
			if (!info) continue;

			
			bool enable = IsAbleToEquip(info, obj, RebirthCountEquipLimit);
			if (enable == parts[a].enable) 
			{
				if( !enable || parts[a].levelConditions.empty() || 
					(parts[a].levelConditions.at(0) > obj->GetLevel()) )
				{
					continue;
				}
				for (std::vector<Stat::IState*>::const_iterator i = parts[a].states.begin() ; i != parts[a].states.end() ; ++i)
				{
					obj->RemoveState((*i)->GetSort(), (*i)->GetId());
				}

				parts[a].states.clear();
				RemoveEquipSetBonus(obj, parts[a].name);
			}
			parts[a].levelConditions.clear();
			parts[a].enable = enable;

			if (enable)
			{
				LogicDBInstance().stateDB.AddItemState(info->Hash, obj, parts[a].states);
				AddEnchantState(info, parts[a].instance, static_cast<XRated::Constants::Equipment>(a), obj, RebirthCountEquipLimit);
			}
			else
			{
				for (std::vector<Stat::IState*>::const_iterator i = parts[a].states.begin() ; i != parts[a].states.end() ; ++i)
				{
					obj->RemoveState((*i)->GetSort(), (*i)->GetId());
				}

				parts[a].states.clear();
				RemoveEquipSetBonus(obj, parts[a].name);
			}

		}
		AddEquipSetBonus(obj);
	}

	bool Equipments::UnEquip(Constants::Equipment fromWhere, uint32& equipmentOld, Player* obj)
	{
		if (parts[fromWhere].name == 0)
			return false;

		return UnEquip(equipmentOld, obj, parts[fromWhere]);
	}

	bool Equipments::UnEquip(uint32& equipmentOld, Player* obj, Equipments::Equipment& equipment)
	{
		equipmentOld = equipment.name;
		for (std::vector<Stat::IState*>::const_iterator i = equipment.states.begin() ; i != equipment.states.end() ; ++i)
			obj->RemoveState((*i)->GetSort(), (*i)->GetId());

		equipment.name = 0;
		equipment.instance = 0;
		equipment.states.clear();
		equipment.levelConditions.clear();

		RemoveEquipSetBonus(obj, equipmentOld);

		return true;
	}

	bool Equipments::ChangeEquipToEquip(Constants::Equipment from, Constants::Equipment to)
	{
		if( (Lunia::XRated::Constants::GetEquipParts(from) != Lunia::XRated::Constants::GetEquipParts(to)) || !IsEnableEquipment(from)) {
			return false;
		}

		Equipment temp = parts[from];
		parts[from] = parts[to];
		parts[to] = temp;
		return true;
	}
	void Equipments::AddEquipSetBonus(Player* obj)
	{
		if(obj->GetCharacterStateFlags().IsEquipSetRewardDisable == true)
			return;

		std::vector<uint32> equipList;
		std::vector<Database::Info::EquipSetManager::RewardInfo> rewardInfo;
			
		GetEquipList(equipList);
		
		DatabaseInstance().InfoCollections.EquipSet.GetRewardEx(rewardInfo,equipList,obj->GetClassType());

		for ( std::vector<Database::Info::EquipSetManager::RewardInfo>::const_iterator i = rewardInfo.begin();
			i != rewardInfo.end(); ++i )
		{
			Player::SetItem* setItemInfo = obj->GetSetItemInfo(i->EquipSet->Id);
			if ( setItemInfo == NULL )
			{
				Player::SetItem tempSetItem(i->EquipSet->Id,0);
				obj->AddSetItemInfo(tempSetItem);
				setItemInfo = obj->GetSetItemInfo(i->EquipSet->Id);
			}

			int curEquipCount = setItemInfo->Count + 1;
			while( curEquipCount <= i->Count )
			{
				Database::Info::EquipSet::RewardCondition rewardCondition;
				rewardCondition.MinimumRequiredCount = curEquipCount;
				rewardCondition.ClassType = obj->GetClassType();

				uint32 rewardHash = i->EquipSet->GetAppropriateRewardHash(rewardCondition);

				const Database::Info::EquipSet::Reward* reward = DatabaseInstance().InfoCollections.EquipSet.GetReward(rewardHash);
				if ( reward != NULL )
				{
					std::pair<uint32, Player::SetItem::Reward > rewards;
					rewards.first = curEquipCount;
					//State Bonus Insert
					for ( std::vector<Database::Info::ActionInfo::State>::const_iterator stateItr = reward->States.begin(); stateItr != reward->States.end(); ++stateItr )
					{
						Database::Info::ActionInfo::State tempState = *stateItr;
						Stat::IState* state = Stat::CreateState(tempState.type, obj, &(tempState.params), &tempState.optionalStates );
						obj->AddState(state);

						rewards.second.states.push_back(state);
					}
					setItemInfo->Rewards.insert(rewards);
				}
				
				++curEquipCount;
            }	

			//Equip Set Info Update
			setItemInfo->Count = i->Count;
		}

	}

	void Equipments::RemoveEquipSetBonus(Player* obj, uint32 equipmentHash)
	{
		if(obj->GetCharacterStateFlags().IsEquipSetRewardDisable == true)
			return;

		//삭제
		const Database::Info::EquipSet::ItemGroup* setItemGroup = DatabaseInstance().InfoCollections.EquipSet.GetEquipItemGroup(equipmentHash);

		if ( ( setItemGroup != NULL && DatabaseInstance().InfoCollections.EquipSet.GetEquipSet(setItemGroup->Id) != NULL ) || DatabaseInstance().InfoCollections.EquipSet.GetJokerEquips(equipmentHash).empty() == false )
		{
			std::vector<uint32> equipList;
			std::vector<Database::Info::EquipSetManager::RewardInfo> rewardInfo;
				
			GetEquipList(equipList);

			DatabaseInstance().InfoCollections.EquipSet.GetRewardEx(rewardInfo,equipList,obj->GetClassType());

			if ( obj->GetSetItemList().size() > rewardInfo.size() )
			{
				std::list<Player::SetItem>& list = obj->GetSetItemList();
				std::list<Player::SetItem>::iterator i = list.begin();
				while( i != list.end() )
				{
					bool bFound = false;
					for ( std::vector<Database::Info::EquipSetManager::RewardInfo>::const_iterator j = rewardInfo.begin(); j != rewardInfo.end(); ++j )
					{
						if ( (*i).Id == (*j).EquipSet->Id )
						{
							bFound = true;
							break;
						}
					}

					if ( bFound == false )
					{
						for ( std::map<uint32, Player::SetItem::Reward >::iterator rewardItr = i->Rewards.begin(); rewardItr != i->Rewards.end(); ++rewardItr )
						{
							for ( std::vector<Stat::IState*>::iterator stateItr = rewardItr->second.states.begin(); stateItr != rewardItr->second.states.end(); ++stateItr )
							{
								obj->RemoveState((*stateItr)->GetSort(), (*stateItr)->GetId());
							}
						}

						i = list.erase(i);
					}
					else
						++i;
                }

			}

			for (auto& i : rewardInfo)
			{	
				Player::SetItem* setItemInfo = obj->GetSetItemInfo(i.EquipSet->Id);
				int curEquipCount = 0;
				if ( setItemInfo != NULL )
				{
					curEquipCount = setItemInfo->Count;
					setItemInfo->Count = i.Count;
				}
				else
				{
					Logger::GetInstance().Warn(L"[Logic::Equipments::UnEquip] Not Exist Infomation of SetItem Equip Data");
					curEquipCount = i.Count;
					return;
				}

				for (; curEquipCount > i.Count; --curEquipCount )
				{
					std::map<uint32, Player::SetItem::Reward >::iterator rewardItr = setItemInfo->Rewards.find(curEquipCount);
					if ( rewardItr != setItemInfo->Rewards.end() )
					{
						for ( std::vector<Stat::IState*>::iterator stateItr = rewardItr->second.states.begin(); stateItr != rewardItr->second.states.end(); ++stateItr )
						{
							obj->RemoveState((*stateItr)->GetSort(), (*stateItr)->GetId());
						}

						setItemInfo->Rewards.erase(rewardItr);
					}
				}

			}
		}
	}

	void Equipments::GetEquipList( std::vector<uint32>& equipList )
	{
		for ( int i = 0; i < XRated::Constants::Equipment::Cnt ; ++i)
		{
			uint32 hash = GetEquipmentId(static_cast<XRated::Constants::Equipment>(i), true);
			if ( hash != 0 )
				equipList.push_back(hash);
		}
	}

	uint32 Equipments::GetPetEquipmentId(XRated::GlobalSerial petSerial, Lunia::XRated::Constants::Equipment where) const
	{
		//입을수 있다.
		int pos = (static_cast<int>(where) - static_cast<int>(XRated::Constants::Equipment::Pet_Mask) );
		if( (pos < 0) && (pos >= Constants::Equipment::MaxPetEquipCnt) ) {
			Logger::GetInstance().Error(L"PetEquipment  GetPetEquipmentId False : have not equipment slot : {0}",where);
			return 0;
		}
		PetsParts::const_iterator iter = std::find_if(petsParts.begin(), petsParts.end(), PetParts::Finder( petSerial ) );
		if( iter == petsParts.end() ) {
			return 0;
		}
		return iter->Parts[pos].name;		
	}
	bool Equipments::IsPetAbleToEquip(const Database::Info::ItemInfo* info, uint32 petHash) const
	{
		const XRated::Database::Info::Pet::Genus* genus = Database::DatabaseInstance().InfoCollections.Pets.GetGenus( petHash );
		if( genus == NULL ) {
			Logger::GetInstance().Exception(L"can not found genus {0}",petHash);
		}
		if( ( XRated::Constants::ClassType::AnyGenus != info->Limits.Class ) && ( genus->EquipClassType != info->Limits.Class ) )
		{
			Logger::GetInstance().Exception(L"PetEquipment IsAableToEquip False : do not not match class type : itemType : [{0}], genus : [{1}]",info->Limits.Class, genus->EquipClassType);
			return false;
		}
		XRated::Constants::Equipment where = XRated::Constants::GetEquipPosition( info->EquipParts );
		int pos = (static_cast<int>(where) - static_cast<int>(XRated::Constants::Equipment::Pet_Mask));
		if( (pos < 0) && (pos >= Constants::Equipment::MaxPetEquipCnt) ) {
			Logger::GetInstance().Error(L"PetEquipment  IsAbleToEquip False : have not equipment slot : {0}", where);
			return false;
		}	
		return true;
	}
	void Equipments::PetEquip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, Player* obj, XRated::GlobalSerial petSerial)
	{
		PetsParts::iterator iter = std::find_if( petsParts.begin(), petsParts.end(), PetParts::Finder( petSerial ) );
		if ( iter == petsParts.end() ) 
		{
			petsParts.push_back( PetParts( petSerial ) );
			iter = std::find_if( petsParts.begin(), petsParts.end() ,PetParts::Finder( petSerial ) );
		}
		int pos = ( static_cast< int >( where ) - static_cast< int >( XRated::Constants::Equipment::Pet_Mask ) );
		if ( ( pos < 0 ) && ( pos >= Constants::Equipment::MaxPetEquipCnt ) ) 
		{
			Logger::GetInstance().Exception(L"PetEquipment  IsAbleToEquip False : have not equipment slot : {0}", where);
		}
		Equipment& equipment = iter->Parts[ pos ];
		Equip( info, fields, equipment, obj );
	}
    bool Equipments::PetUnEquip(Constants::Equipment fromWhere, uint32& equipmentOld, Player* obj, XRated::GlobalSerial petSerial)
	{
		PetsParts::iterator iter = std::find_if(petsParts.begin(), petsParts.end(), PetParts::Finder( petSerial ) );
		if( iter != petsParts.end() ) {
			PetUnEquip(fromWhere, equipmentOld, obj, (*iter) );
			return true;
		}
		return false;
	}
	void Equipments::PetUnEquipAll(XRated::GlobalSerial petSerial, Player* obj)
	{
		uint32 equipmentOld;
		PetsParts::iterator iter = std::find_if(petsParts.begin(), petsParts.end(), PetParts::Finder( petSerial ) );
		if( iter != petsParts.end() ) {
			for( size_t i =0; i < XRated::Constants::Equipment::MaxPetEquipCnt; ++i )
			{				
				PetUnEquip( static_cast<XRated::Constants::Equipment>(i + XRated::Constants::Equipment::Pet_Mask), equipmentOld, obj, (*iter) );
			}
		}
	}
	bool Equipments::PetUnEquip(Constants::Equipment fromWhere, uint32& equipmentOld, Player* obj, PetParts& petParts)
	{
		int pos = (static_cast<int>(fromWhere)-XRated::Constants::Pet_Mask);
		if( (pos < 0) && (pos >= Constants::Equipment::MaxPetEquipCnt) ) {
			return false;
		}
		Equipment& equipment = petParts.Parts[pos];
		if( equipment.name == 0 ) {
			return false;
		}
		UnEquip(equipmentOld, obj, equipment);
		
		return true;
	}
	

	void Equipments::Equip(const Database::Info::ItemInfo* info, int64 fields, Equipments::Equipment& equipment, Player* obj, uint16 rebirthCountEquipLimit )
	{
		equipment.name = info->Hash;
		equipment.instance = fields;
		LogicDBInstance().stateDB.AddItemState(info->Hash, obj, equipment.states);

		//Enchant처리
		//Database::Enchant::EnchantBitfields

		if ( Enchant::IsEnchantable(info) ) {
			std::vector<Database::Info::ActionInfo::State> states;
			Enchant::GetItemInstanceForServer(info, fields, states);
			std::vector<Database::Info::ActionInfo::State>::iterator i = states.begin();
			std::vector<Database::Info::ActionInfo::State>::iterator iEnd = states.end(); 
			for ( ; i != iEnd ; ++i ) {
				Stat::IState* state = Stat::CreateState((*i).type, obj, &(*i).params, &(*i).optionalStates);
				obj->AddState(state);
				equipment.states.push_back(state);
			}
		} else {
			//아바타 아이템
			Enchant::NormalBitfields normal(fields);
			std::pair< Enchant::CashEnchantInfo*, Enchant::CashEnchantInfo* > info = DatabaseInstance().InfoCollections.EnchantTables.GetCashEnchantInfo(normal.CashEnchant1, normal.CashEnchant2);
			if (info.first && DatabaseInstance().InfoCollections.EnchantTables.IsCashEnchantAvailable(info.first, obj->GetLevel(), obj->GetStoredLevel(), obj->GetRebirthCount(), rebirthCountEquipLimit) ) 
			{
				std::vector<Database::Info::ActionInfo::State>::iterator i = info.first->States.begin();
				std::vector<Database::Info::ActionInfo::State>::iterator iEnd = info.first->States.end(); 
				for ( ; i != iEnd ; ++i ) {
					Stat::IState* state = Stat::CreateState((*i).type, obj, &(*i).params, &(*i).optionalStates);
					obj->AddState(state);
					equipment.states.push_back(state);
				}
			}
			if (info.second && DatabaseInstance().InfoCollections.EnchantTables.IsCashEnchantAvailable(info.second, obj->GetLevel(), obj->GetStoredLevel(), obj->GetRebirthCount(), rebirthCountEquipLimit) ) 
			{
				std::vector<Database::Info::ActionInfo::State>::iterator i = info.second->States.begin();
				std::vector<Database::Info::ActionInfo::State>::iterator iEnd = info.second->States.end(); 
				for ( ; i != iEnd ; ++i ) {
					Stat::IState* state = Stat::CreateState((*i).type, obj, &(*i).params, &(*i).optionalStates);
					obj->AddState(state);
					equipment.states.push_back(state);
				}
			}
		}
	}

	Equipments::PetParts::PetParts(XRated::GlobalSerial petSerial) : PetSerial(petSerial)
	{
	}

}	}	}
