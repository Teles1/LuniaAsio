#include "NoLogicItems.h"
#include "../UserManager.h"
#include <StageServer/User/User.h>
#include <StageServer/User/IUserRoom.h>
#include <Info/Info/InfoHelper.h>
#include <Core/ErrorDefinition.h>
#include "Enchanter.h"
#include <Core/Utils/TimeSpan.h>

namespace Lunia { namespace XRated { namespace StageServer { 

	const Database::Info::ActionInfo::State& NonLogicItem::GetNoLogicState(const Database::Info::ItemInfo* info)
	{
		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());
		if (i==info->States.end())
			throw InvalidRuntimeParameterException(L"unable to find NOLOGIC state"); 

		return *i;
	}

	const wchar_t* NonLogicItem::GetNoLogicTypeString(const Database::Info::ItemInfo* info)
	{
		/* retrieve nologic state */
		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());
		if (i==info->States.end())
			return NULL;

		/* retrieve */
		std::map<String, String>::const_iterator keyValue=i->params.find(L"Type");
		if (keyValue==i->params.end())
			return NULL;

		return keyValue->second.c_str();
	}



	/* TODO : optimization, static information (Database::Info::ItemInfo) parameters can be validated earlier like at binary converting. */


	/* BankBag ***********************************************************************************/
	BankBag::BankBag(UserSharedPtr sender, Items& items)
		: sender(sender), items(items)
	{
	}

	const wchar_t* BankBag::GetTypeName() const
	{
		return L"BankBag";
	}

	uint16 BankBag::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		if (parameters.size()!=1)
			throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for BankBag" );

		uint32 bagNumber=StringUtil::ToInt(parameters.at(0));
		if( bagNumber < 0 ) {
			throw InvalidRuntimeParameterException( L"out of bag number range(lower than zero)");
		}
		/*if (bagNumber>=XRated::Constants::MaxOfBankBags)
			throw InvalidRuntimeParameterException( L"out of bag number range" )) );
		*/
		if (items.IsEnable( true, bagNumber ) && items.GetExpireDate(true, bagNumber)==DateTime::Infinite)
			throw InvalidRuntimeParameterException( L"already unlimitted BankBag");

		std::map<String, String>::const_iterator keyValue=i->params.find(L"Day");
		if (keyValue==i->params.end())
			throw InvalidRuntimeParameterException( fmt::format(L"BankBag item needs Day parameter. wrong item info({}:{})", info->Hash, info->Id.c_str() ).c_str() );

		if (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)
			return parameters.size();

		return 0;
	}

	void BankBag::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		std::map<String, String>::const_iterator keyValue;

		/* parameter must be a bag number */
		uint32 bagNumber=StringUtil::ToInt(parameters.at(0));
		keyValue=i->params.find(L"Day");
		int days=StringUtil::ToInt(keyValue->second);

		/*  renew the bag and update DB */
		DateTime nextExpireDate = items.RenewBag(true, bagNumber, days);
		Net::Api request("BankBags/Update");
		request << sender->GetCharacterName() << bagNumber << nextExpireDate;
		request.GetAsync();
		/* notice to client */
		Protocol::FromServer::ChangeBagState packet;
		packet.IsBank=true;
		packet.Bag.BagNumber=bagNumber;
		packet.Bag.ExpireDate=nextExpireDate;
		packet.Bag.Expired=false;
		sender->ItemsSend( packet );
		/*if(days == 0){ // only unlimited
			User* user=UserManagerInstance().GetUser(characterName);
			if(user->achievement.IsConnected()){
				Net::Protocol::Achievement::ServerProtocol::BankBagOpened packetone;
				packetone.characterName = user->GetName();
				packetone.openBagCount = static_cast<uint32>(items.GetBagsSlots(true)); // idk this, ??w
				user->achievement.Send(packetone);
			}
		}
		*/
	}
	/*********************************************************************************** BankBag */



	/* InventoryBag ******************************************************************************/
	InventoryBag::InventoryBag(UserSharedPtr sender, Items& items)
		: sender(sender), items(items)
	{
	}

	const wchar_t* InventoryBag::GetTypeName() const
	{
		return L"InventoryBag";
	}

	uint16 InventoryBag::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		if (parameters.size()!=1)
			throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for InventoryBag" );

		uint32 bagNumber=StringUtil::ToInt(parameters.at(0));		
		if( bagNumber < 1 ) {
			throw InvalidRuntimeParameterException( L"out of bag number range(to zero)" );
		}
		/*if (bagNumber>XRated::Constants::MaxOfInventoryBags) // not >= because bagNumber 0 is reserved as equipment.
			throw InvalidRuntimeParameterException( L"out of bag number range" )) );*/
		if (items.IsEnable( false, bagNumber ) && items.GetExpireDate(false, bagNumber)==DateTime::Infinite)
			throw InvalidRuntimeParameterException( L"already unlimitted InventoryBag" );

		std::map<String, String>::const_iterator keyValue=i->params.find(L"Day");
		if (keyValue==i->params.end())
			throw InvalidRuntimeParameterException( fmt::format(L"InventoryBag item needs Day parameter. wrong item info({0}:{1})", info->Hash, info->Id.c_str()).c_str());

		if (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)
			return parameters.size();

		return 0;
	}

	void InventoryBag::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		std::map<String, String>::const_iterator keyValue;

		/* parameter must be a bag number */
		uint32 bagNumber=StringUtil::ToInt(parameters.at(0));
		keyValue=i->params.find(L"Day");
		int days=StringUtil::ToInt(keyValue->second);

		/*  renew the bag and update DB */
		DateTime nextExpireDate = items.RenewBag(false, bagNumber, days);
		Net::Api request("UpdateBagState");
		request << sender->GetCharacterName() << "0" << bagNumber << nextExpireDate;
		request.GetAsync();

		/* notice to client */
		Protocol::FromServer::ChangeBagState packet;
		packet.IsBank=false;
		packet.Bag.BagNumber=bagNumber;
		packet.Bag.ExpireDate=nextExpireDate;
		packet.Bag.Expired=false;
		sender->ItemsSend( packet );
		/*User* user=UserManagerInstance().GetUser(characterName);
		if(user->IsAchievementConnected()){			
			Net::Protocol::Achievement::ServerProtocol::InventoryBagOpened pkt;
			pkt.characterName = sender.GetName();
			pkt.openBagCount = items.GetBagsSlots(false);
			sender.ItemsSendAchievement(pkt);
		}*/

		/* stage item processing */
		items.RemoveStageItems();
	}
	/****************************************************************************** InventoryBag */



	/* SkillReset ********************************************************************************/
	SkillReset::SkillReset(XRated::Logic::Player*& player, std::shared_ptr<IUserRoom>& room)
		: player(player), room(room)
	{
	}

	const wchar_t* SkillReset::GetTypeName() const
	{
		return L"SkillReset";
	}

	uint16 SkillReset::GetConsumableCount(const Database::Info::ItemInfo* /*info*/, const std::vector<String>& parameters) const
	{
		if (parameters.empty()) 
			throw InvalidRuntimeParameterException( L"SkillReset item must have runtime parameter to UseItemEx");

		/* every parameter must be skill hash which the user have */
		std::vector<XRated::Skill> skills=player->GetPlayerData().Skills;
		for(std::vector<String>::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
		{
			std::vector<XRated::Skill>::iterator skill=std::find_if( skills.begin(), skills.end()
				, XRated::Skill::Finder(StringUtil::ToInt(*i)) );
			if (skill==skills.end())
			{
				throw InvalidRuntimeParameterException( fmt::format(L"wrong skill hash :{0}", *i).c_str());
			}
			else if (skill->Level==0)
			{
				throw InvalidRuntimeParameterException(fmt::format(L"already 0 skill level : {0}", *i).c_str());
			}
			else
			{
				--skill->Level;
			}
		}

		return parameters.size();
	}

	void SkillReset::Use(const Database::Info::ItemInfo* /*info*/, const std::vector<String>& parameters)
	{
		for(std::vector<String>::const_iterator i=parameters.begin(); i!=parameters.end(); ++i)
		{
			std::vector<XRated::Skill>::iterator skill=std::find_if( player->GetPlayerData().Skills.begin(), player->GetPlayerData().Skills.end()
				, XRated::Skill::Finder(StringUtil::ToInt(*i)) );
			if (skill->Level)
			{
				room->DecreaseSkillPoint(player, StringUtil::ToInt(*i));
			}
		}
	}
	/******************************************************************************** SkillReset */

	/* CharacterLicense ********************************************************************************/
	CharacterLicense::CharacterLicense(UserSharedPtr pUser)
		: user(pUser)
	{
	}

	const wchar_t* CharacterLicense::GetTypeName() const
	{
		return L"CharacterLicense";
	}

	uint16 CharacterLicense::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		//if (parameters.empty()) 
		//	throw InvalidRuntimeParameterException( L"CharacterLicense item must have runtime parameter to UseItemEx" )) );

		Constants::ClassType classType( (Constants::ClassType)GetNoLogicStateValue(info, L"ClassType", -1) );

		if( (int)classType == -1)
		{
			throw InvalidRuntimeParameterException( L"classtype param does not exist");
		}
		else if ( classType < Constants::ClassType::Healer || classType >= Constants::ClassType::NumberOfClassTypes )
		{
			throw InvalidRuntimeParameterException( fmt::format(L"wrong class type : {0}", classType ).c_str());
		}
		else if ( user->HasCharacterLicense( classType ) == true ) 
		{
            return 0;	//user already has license.
		}

		return 1;
	}

	void CharacterLicense::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		Constants::ClassType classType( (Constants::ClassType)GetNoLogicStateValue(info, L"ClassType", -1) );

		if( (int)classType == -1)
		{
			throw InvalidRuntimeParameterException( L"classtype param does not exist");
		}
		else if ( classType < Constants::ClassType::Healer || classType >= Constants::ClassType::NumberOfClassTypes )
		{
			throw InvalidRuntimeParameterException( fmt::format(L"wrong class type : {0}", classType).c_str() );
		}

		user->GiveCharacterLicense(classType);
		//room->GiveCharacterLicense(player, classType);
	}
	/******************************************************************************** CharacterLicense */

	/* SkillResetAll *****************************************************************************/
	SkillResetAll::SkillResetAll(XRated::Logic::Player*& player, std::shared_ptr<IUserRoom>& room)
		: player(player), room(room)
	{
	}

	const wchar_t* SkillResetAll::GetTypeName() const
	{
		return L"SkillResetAll";
	}

	uint16 SkillResetAll::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		if (parameters.empty()==false) 
			throw InvalidRuntimeParameterException( L"SkillResetAll item must have no runtime parameter to UseItemEx" );

		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;		
	}

	void SkillResetAll::Use(const Database::Info::ItemInfo* /*info*/, const std::vector<String>& /*parameters*/)
	{
		room->ResetSkillPoint(player, 0/*means all of skills */);
	}
	/***************************************************************************** SkillResetAll */


	/* Nothing ***********************************************************************************/
	Nothing::Nothing()
	{
	}

	const wchar_t* Nothing::GetTypeName() const
	{
		return L"Nothing";
	}

	uint16 Nothing::GetConsumableCount(const Database::Info::ItemInfo* /*info*/, const std::vector<String>& /*parameters*/) const
	{
		return 1;
	}

	void Nothing::Use(const Database::Info::ItemInfo* /*info*/, const std::vector<String>& /*parameters*/)
	{
	}
	/*********************************************************************************** Nothing */


	/* EnchantRecover ****************************************************************************/
	EnchantRecover::EnchantRecover(UserSharedPtr user, Items& items)
		: user(user), items(items)
	{
	}

	const wchar_t* EnchantRecover::GetTypeName() const
	{
		return L"EnchantRecover";
	}

	uint16 EnchantRecover::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		if (parameters.size()<2)
			throw InvalidRuntimeParameterException( L"not enough parameters to recover enchant");

		int fee (GetNoLogicStateValue(info, L"Fee", 0));
		int target (GetNoLogicStateValue(info, L"Target", 0));
		int count (GetNoLogicStateValue(info, L"ConsumableCount", 1));
		int stageLevelLimit (GetNoLogicStateValue(info, L"LimitStageLevel", 99));
		int pvpLevelLimit (GetNoLogicStateValue(info, L"LimitPvpLevel", 99));

		if (static_cast<uint32>(user->GetMoney()) < fee)
			throw InvalidRuntimeParameterException( L"not enough money to recover enchant");

		if ((target & TargetFlags::InvalidIdentify)==TargetFlags::InvalidIdentify ||
			(target & TargetFlags::InvalidReinforcement)==TargetFlags::InvalidReinforcement ||
			(target & TargetFlags::InvalidLightReinforcement)==TargetFlags::InvalidLightReinforcement)
			throw InvalidRuntimeParameterException( L"invalid Target specified. target cannot be consisted by exclusive elements");

		if ((target & TargetFlags::IdentifyOne) && parameters.size()<3) // identify-one needs one more parameter for identify index
			throw InvalidRuntimeParameterException( L"not enough parameters to recover enchant - identify");

		const Common::ItemEx* item=items.GetItem( StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]) );
		if (item==NULL)
			throw InvalidRuntimeParameterException( L"invalid item position to recover enchant");
		
		if (Database::Enchant::IsEnchantable(item->Info)==false)
			throw InvalidRuntimeParameterException( fmt::format(L"non-enchantabe item specified (hash:{0})", item->Info->Hash).c_str());

		if (item->Info->Limits.MaxLevel < stageLevelLimit)
			throw InvalidRuntimeParameterException(fmt::format(L"item({}) level({}) is too low(should be greater than {}) to be recovered", item->Info->Hash, item->Info->Limits.MaxLevel, stageLevelLimit).c_str());

		if (item->Info->Limits.MaxPvpLevel < pvpLevelLimit)
			throw InvalidRuntimeParameterException(fmt::format(L"item({}) pvplevel({}) is too low(should be greater than {}) to be recovered", item->Info->Hash, item->Info->Limits.MaxPvpLevel, pvpLevelLimit ).c_str());

		/* TODO : validation to detect recoverable status */

		//Database::Enchant::EnchantBitfields enchant(item->Instance);
		//if ( (target & TargetFlags::IdentifyOne) && ( enchant.Identified1==0 && enchant.Identified2==0 && enchant.Identified3==0 ) )
		//	throw InvalidRuntimeParameterException( L"non-recoverable item specified (instance:{})", item->Instance )) );


		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?static_cast<uint16>(count):0;
	}

	void EnchantRecover::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		const Common::ItemEx* item=items.GetItem( StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]) );
		int fee (GetNoLogicStateValue(info, L"Fee", 0));
		int target(GetNoLogicStateValue(info, L"Target", 0));

		Database::Enchant::EnchantBitfields instance(item->InstanceEx);

		if (target & TargetFlags::IdentifyAll) { instance.Identified=0; }
			//if (target & TargetFlags::IdentifyOne)
			//{
			//	switch( parameters[2] )
			//	{
			//	case 0: instance.Identified1=0; break;
			//	case 1: instance.Identified2=0; break;
			//	case 2: instance.Identified3=0; break;
			//	}
			//}

			//if (target & TargetFlags::IdentifyAll)
			//{
			//	instance.Identified1=0;
			//	instance.Identified2=0;
			//	instance.Identified3=0;
			//}

		if (target & TargetFlags::ReinforcementRecover)
		{
			instance.DestroyedByReinforcement=0;
		}
		
		if (target & TargetFlags::ReinforcementReset)
		{
			instance.DestroyedByReinforcement=0;
			instance.Reinforcement=0;
		}

		if (target & TargetFlags::LightReinforcementRecover)
		{
			instance.DestroyedByLightReinforcement=0;
		}

		if (target & TargetFlags::LightReinforcementReset)
		{
			instance.DestroyedByLightReinforcement=0;
			instance.LightLevel=0;
		}
		
		Common::ItemEx result(*item);
		result.InstanceEx=instance;
		items.SetItem( StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]), result);

		/* TODO : check which packet is better to be processed by client */
		Protocol::FromServer::Identify packet;
		packet.Position = XRated::ItemPosition(static_cast<uint8>(StringUtil::ToInt(parameters[0])), static_cast<uint8>(StringUtil::ToInt(parameters[1])));
		packet.ItemHash=result.Info->Hash;
		packet.instanceEx=instance;
        user->Send( packet );        

		user->SetMoney( user->GetMoney() - fee );
		user->UpdateInfos();
	}
	/**************************************************************************** EnchantRecover */


	/* PlayerStore *******************************************************************************/
	PlayerStore::PlayerStore(UserSharedPtr user, Items& items, PlayerStoreManager& playerStore)
		: user(user), items(items), playerStore(playerStore)
	{
	}

	const wchar_t* PlayerStore::GetTypeName() const
	{
		return L"PlayerStore";
	}

	uint16 PlayerStore::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		int titleLength(GetNoLogicStateValue(info, L"TitleLength", 10));
		int slotCount(GetNoLogicStateValue(info, L"SlotCount", 4));
		int fee(GetNoLogicStateValue(info, L"Fee", 0));
		//float effectiveRange=StringUtil::ToFloat( stateParameters.find(L"EffectiveRange")->second );

		/* validate */
		if (user->GetState() != User::ACTIVE)
			throw InvalidRuntimeParameterException(fmt::format(L"invalid user state for player({})", user->GetState() ).c_str() );

		if (parameters.size() < 1/*title*/+(3*MinimalNumberOfSlots))
			throw InvalidRuntimeParameterException( L"not enough parameter for player store" );

		/* TODO : check the number of stores in the room */

		std::string title=StringUtil::ToASCII( parameters.at(0) );
		if ( title.size() > static_cast<uint32>(titleLength) ) // title length should be 
			throw InvalidRuntimeParameterException( fmt::format(L"invalid length of the store title ({}/{})", title.size(), titleLength ).c_str() );

		/* TODO : calculate effectiveRange */
		float3 location(user->GetPlayer()->GetPosition());

		if ((parameters.size()-1) % 3 )
			throw InvalidRuntimeParameterException(fmt::format(L"invalid size of parameters ({})", parameters.size() ).c_str() );
		if ( ((parameters.size()-1)/3) > static_cast<uint32>(slotCount) )
			throw InvalidRuntimeParameterException(fmt::format(L"invalid size of slot count for player store ({}/{})", (parameters.size()-1)/3, slotCount ).c_str() );
        if ( static_cast<uint32>(user->GetMoney()) < fee )
			throw InvalidRuntimeParameterException(fmt::format(L"not enough money for player store ({}/{})", user->GetMoney(), fee ).c_str() );

		uint32 i(0);
		ItemPosition pos;
		while( i < parameters.size()-1 )
		{
			uint8 bag = static_cast<uint8>(StringUtil::ToInt( parameters[++i] ));
			uint8 pos = static_cast<uint8>(StringUtil::ToInt( parameters[++i] ));
			if (bag==0)
				throw InvalidRuntimeParameterException( L"item should not be in equipment");

			const Common::ItemEx* item = items.GetItem( bag, pos );
			if (item==NULL || 
				item->IsTradable(user->shared_from_this())==false )
			{
				throw InvalidRuntimeParameterException(fmt::format(L"invalid item requested ({})", item?item->Info->Hash:0 ).c_str());
			}
			++i; // skip money validation
		}

		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void PlayerStore::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		/* calculate parameters */
		float commissionRate(GetNoLogicStateValue(info, L"CommissionRate", 0.0f));
		float discountRate(GetNoLogicStateValue(info, L"DiscountRate", 0.0f));
		int fee(GetNoLogicStateValue(info, L"Fee", 0));

		std::vector<StoreSlot> slots;
		std::vector<StoreSlotForPet> slotsForPet;
		slots.reserve((parameters.size()-1)/3);

		/* making slots */
		uint32 i(0);
		StoreSlot slot;
		StoreSlotForPet petSlot;
		AutoLock lock(user->GetPetDatas().GetSyncObject());
		while( i < parameters.size()-1 )
		{
			uint8 bag = static_cast<uint8>(StringUtil::ToInt( parameters[++i] ));
			uint8 pos = static_cast<uint8>(StringUtil::ToInt( parameters[++i] ));
			const Common::ItemEx* item = items.GetItem( bag, pos );

			if(Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(item->Info->Hash) == false)
			{
				slot.InstanceEx = item->InstanceEx;
				slot.ItemHash = item->Info->Hash;
				slot.StackedCount = item->StackedCount;
				slot.SellPrice = StringUtil::ToInt( parameters[++i] );
				slots.push_back(slot);
			}else {					
				const XRated::Pet* pet = user->GetPetDatas().GetPetData( item->Serial );
				if( pet) {
					petSlot.InstanceEx = item->InstanceEx;
					petSlot.ItemHash = item->Info->Hash;
					petSlot.StackedCount = item->StackedCount;
					petSlot.SellPrice = StringUtil::ToInt( parameters[++i] );
					petSlot.PetData = *pet;
					slotsForPet.push_back(petSlot);
				}else {
					slot.InstanceEx = item->InstanceEx;
					slot.ItemHash = item->Info->Hash;
					slot.StackedCount = item->StackedCount;
					slot.SellPrice = StringUtil::ToInt( parameters[++i] );
					slots.push_back(slot);
				}				
			}
		}

		user->SetMoney( user->GetMoney() - fee );
		playerStore.Open(user->shared_from_this(), items, *info, commissionRate, discountRate, parameters[0], slots, slotsForPet);
	}
	/******************************************************************************* PlayerStore */

	/* CharacterSlot *****************************************************************************/
	CharacterSlot::CharacterSlot(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* CharacterSlot::GetTypeName() const
	{
		return L"CharacterSlot";
	}

	uint16 CharacterSlot::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/) const
	{
		int fee(GetNoLogicStateValue(info, L"Fee", 0));

		/* validate */		 
        if ( user->GetMoney() < static_cast<uint32>(fee) )
			throw InvalidRuntimeParameterException( fmt::format(L"not enough money for new character slot ({}/{})", user->GetMoney(), fee ).c_str() );
		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void CharacterSlot::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	{
		int slots(GetNoLogicStateValue(info, L"NumberOfSlots", 1));
		int fee(GetNoLogicStateValue(info, L"Fee", 0));

		user->SetMoney( user->GetMoney() - fee );

		Net::Api packet("AddCharacterSlot");
		packet << user->GetName() << slots;
		packet.GetAsync(this, &CharacterSlot::Extended, user->shared_from_this());
	}
	void CharacterSlot::Extended(const UserSharedPtr& user, const Net::Answer& answer)
	{
		if (answer.errorCode == 0)
		{
			Protocol::FromServer::CharacterSlot response;
			answer.resultObject.at("NumberOfSlots").get_to(response.NumberOfSlots);
			user->Send(response);
		}
	}
	/***************************************************************************** CharacterSlot */


	/* Megaphone *********************************************************************************/
	Megaphone::Megaphone(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* Megaphone::GetTypeName() const
	{
		return L"Megaphone";
	}

	uint16 Megaphone::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		if (parameters.size() < 1)
			throw InvalidRuntimeParameterException( L"not enough parameter");

		int messageLength(GetNoLogicStateValue(info, L"MessageLength", 20));
		if ( StringUtil::ToASCII(parameters[0]).size() > static_cast<uint32>(messageLength) )
			throw InvalidRuntimeParameterException( L"message is too long");

		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void Megaphone::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		bool shoutToSquares(GetNoLogicStateValue(info, L"ShoutToSquares", 1)?true:false);
		bool shoutToStages(GetNoLogicStateValue(info, L"ShoutToStages", 0)?true:false);

		if (shoutToSquares)
			;// InputMgr::GetGeneralController().RequestTo(Service::SquareService, L"chat", user->GetName() + L" : " + parameters[0]);
		if (shoutToStages)
			;// InputMgr::GetGeneralController().RequestTo(Service::StageService, L"chat", user->GetName() + L" : " + parameters[0]);
	}
	/********************************************************************************* Megaphone */

	/* TextBoardMessage **************************************************************************/
	TextBoardMessage::TextBoardMessage( UserSharedPtr user )
		: user( user )
	{
	}

	const wchar_t* TextBoardMessage::GetTypeName() const
	{
		return L"TextBoardMessage";
	}

	uint16 TextBoardMessage::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		if (parameters.size() < 1)
			throw InvalidRuntimeParameterException( L"not enough parameter");

		int messageLength(GetNoLogicStateValue(info, L"MessageLength", 80));
		if ( StringUtil::ToASCII(parameters[0]).size() > static_cast<uint32>(messageLength) )
			throw InvalidRuntimeParameterException( L"message is too long" );

		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void TextBoardMessage::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		bool shoutToSquares(GetNoLogicStateValue(info, L"ShoutToSquares", 1)?true:false);
		bool shoutToStages(GetNoLogicStateValue(info, L"ShoutToStages", 0)?true:false);

		int  repeat(GetNoLogicStateValue(info, L"Repeat", -1));
		int  playTime(GetNoLogicStateValue(info, L"PlayTime", 0));
		/*
		if (shoutToSquares)
			InputMgr::GetGeneralController().RequestTo( Service::SquareService, L"textBoardMsg",
			StringUtil::ToUnicode( repeat ) + L" " + StringUtil::ToUnicode( playTime ) + L" " + 
			user->GetName() + L" " + parameters[0] ); // "�ð�(��) �����̸� : �޼���"
		if (shoutToStages)
			InputMgr::GetGeneralController().RequestTo( Service::StageService, L"textBoardMsg",
			StringUtil::ToUnicode( repeat ) + L" " + StringUtil::ToUnicode( playTime ) + L" " + 
			user->GetName() + L" " + parameters[0] ); // "�ð�(��) �����̸� : �޼���"
		*/
	}
	/************************************************************************** TextBoardMessage */

	/* Store *************************************************************************************/
	Store::Store(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* Store::GetTypeName() const
	{
		return L"Store";
	}

	uint16 Store::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/) const
	{
		int shopType(GetNoLogicStateValue(info, L"ShopType", 0));
		if (shopType<0 || shopType>XRated::Constants::ShopTypeCnt)
			throw InvalidRuntimeParameterException( fmt::format(L"invalid shop type of the item({})", shopType ).c_str() );
        
		bool useInSquare(GetNoLogicStateValue(info, L"UseInSquare", 0)?true:false);
		if (useInSquare==false && user->GetRoom()->GetRoomKind()==Common::SQUARE)
			throw InvalidRuntimeParameterException( L"unable to use this item in square");

		bool useInStage(GetNoLogicStateValue(info, L"UseInStage", 0)?true:false);
		if (useInSquare==false && useInStage==false && user->GetRoom()->IsNowCampfire()==false)
			throw InvalidRuntimeParameterException( L"unable to use this item in stage" );

		bool useInCampfire(GetNoLogicStateValue(info, L"UseInCampfire", 1)?true:false);
		if (useInCampfire==false && user->GetRoom()->IsNowCampfire()==true)
			throw InvalidRuntimeParameterException( L"unable to use this item in camp fire" );

		int fee(GetNoLogicStateValue(info, L"Fee", 0));
		if (static_cast<uint32>(user->GetMoney()) < fee)
			throw InvalidRuntimeParameterException( L"not enough money"  );

		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void Store::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	{
		int fee(GetNoLogicStateValue(info, L"Fee", 0));
		user->SetMoney( user->GetMoney() - fee );

		int shopType(GetNoLogicStateValue(info, L"ShopType", 0));
		user->SetState( User::SHOP );
		user->GetRoom()->EnterShop( user->GetPlayer()->GetSerial(), static_cast<XRated::Constants::ShopType>(shopType), 0	);
	}
	/************************************************************************************* Store */

	/* QuestActivity *************************************************************************************/
	QuestActivity::QuestActivity(UserSharedPtr user, QuestManager& quest)
		: user(user)
		, quest(quest)
	{
	}

	const wchar_t* QuestActivity::GetTypeName() const
	{
		return L"QuestActivity";
	}

	uint16 QuestActivity::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/) const
	{
		uint32 questHash(GetNoLogicStateValue(info, L"QuestHash", 0));
		if (questHash == 0)
			throw InvalidRuntimeParameterException( fmt::format(L"not include questHash in the itemInfo({})", info->Hash).c_str() );

		return 0; // Quest Active Item shoule be always 0
	}

	void QuestActivity::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	{
		uint32 questHash(GetNoLogicStateValue(info, L"QuestHash", 0));
		quest.ValidActiveItem(user->shared_from_this(), questHash);
	}
	/************************************************************************************* QuestActiveItem */

	/* ReviveStone *****************************************************************************************/
	ReviveStone::ReviveStone(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* ReviveStone::GetTypeName() const
	{
		return L"ReviveStone";
	}

	uint16 ReviveStone::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{	
		std::cout << "ReviveStone::GetConsumableCount" << std::endl;
		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		if (parameters.size()!=1)
			throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for Revive" );

		uint32 playerSerial =StringUtil::ToInt(parameters.at(0));
		Database::Info::FindReviveState condition(Database::Info::StateInfo::Type::REVIVE);

		if(user->GetRoom())
		{
			auto targetUser = user->GetRoom()->GetUser(playerSerial);
			if( targetUser != NULL )
			{
				auto room = targetUser->GetRoom();
				if( room)
				{
					Database::Info::StageInfo* stageInfo = room->GetStageInfo();
					if( stageInfo && stageInfo->isReviveAvailable == false )
					{
						return 0;
					}
				}
			}

			if(!targetUser || !targetUser->GetPlayer() || targetUser->GetPlayer()->GetPlayerData().Life != 0 || !condition(*info)
				|| !targetUser->GetPlayer()->CheckState(Database::Info::StateInfo::GHOST))
				return	0;
		}
		else
			return	0;

		return 1;
	}

	void ReviveStone::Use(const Database::Info::ItemInfo* /*info*/, const std::vector<String>& parameters)
	{
		std::cout << "ReviveStone::Use" << std::endl;
		uint32 playerSerial=StringUtil::ToInt(parameters.at(0));

		auto targetUser = user->GetRoom()->GetUser(playerSerial);

		if(!user->GetRoom() || !targetUser)
			return;
		
		std::cout << "ReviveStone::Use" << std::endl;
		user->GetRoom()->Revive(targetUser->GetPlayer(), false);		
	}
	/***************************************************************************************** ReviveStone */

	/* DailyItem *********************************************************************************/

	
	DailyItem::DailyItem(UserSharedPtr user,Items& items) : user(user),items(items)
	{
	}

	const wchar_t* DailyItem::GetTypeName() const
	{
		return L"DailyItem";
	}
	uint16 DailyItem::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		if( parameters.size() != 3 ) {
			throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for DailyItem" );
		}
		if( Database::Enchant::IsEnchantable(info) == true ) {
			throw InvalidRuntimeParameterException( fmt::format(L"DailyItem : this item is IsEnchantable True : ItemInfo({})",info->Hash).c_str() );
		}
		const Common::ItemEx* item=items.GetItem( StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]),false );
		if (item==NULL) {
			throw InvalidRuntimeParameterException( L"invalid item position to recover enchant" );
		}
		if( Database::Enchant::IsHaveTodayDailyItem(user->GetConnectDate(),item->InstanceEx) == false ) {
			throw InvalidRuntimeParameterException( L"DailyItem : impossible request" );
		}
		const Database::Info::DailyItem* dailyItemInfo = Database::DatabaseInstance().InfoCollections.DailyItems.Get(info->Hash, user->GetConnectDayOfWeek(),StringUtil::To<uint32>(parameters[2]));
		if( dailyItemInfo == NULL  ) {
			throw InvalidRuntimeParameterException( fmt::format(L"can not found daily item info {}, {}", user->GetConnectDayOfWeek(),StringUtil::To<uint32>(parameters[2]) ).c_str() );
		}
		uint32 slotCount = dailyItemInfo->SlotCount;
		const Database::Info::ItemInfo* acquireItemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve( dailyItemInfo->ItemHash );
		if( acquireItemInfo == NULL ) {
			throw InvalidRuntimeParameterException( fmt::format(L"can not found acquir item info {}", dailyItemInfo->ItemHash ).c_str() );
		}
		if( (acquireItemInfo->ItemType != Database::Info::ItemInfo::Type::EQUIPMENT) && (acquireItemInfo->ItemType != Database::Info::ItemInfo::Type::UNIDENTIFIED) ) {			
			slotCount = (dailyItemInfo->SlotCount/acquireItemInfo->MaxCnt);
			if( (dailyItemInfo->SlotCount % acquireItemInfo->MaxCnt) != 0 ) {
				slotCount += 1;
			}
		}
		if( user->GetBlankSlotCount() < slotCount ) {
			throw InvalidRuntimeParameterException(fmt::format(L"need more inventory slot : {}", static_cast<int>(slotCount) ).c_str() );
		}
		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;				
	}
	void DailyItem::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		const Database::Info::DailyItem* dailyItemInfo = Database::DatabaseInstance().InfoCollections.DailyItems.Get(info->Hash, user->GetConnectDayOfWeek(),StringUtil::To<uint32>(parameters[2]));
		const Common::ItemEx* item=items.GetItem( StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]),false );
		if( item != NULL ) {
			Database::Enchant::DailyItemLastAcquiredDate	date;
			date.Year = (user->GetConnectDate().GetYear() & 2047 );
			date.Month = (user->GetConnectDate().GetMonth() & 15);
			date.Day = user->GetConnectDate().GetDay() & 31;

			Database::Enchant::NormalBitfields instance = item->InstanceEx.Instance;
			instance.CashEnchant1 = date.Year;
			instance.CashEnchant2 = (date.Month<<5) | date.Day;
			Common::ItemEx result(*item);
			result.InstanceEx=instance;
			items.SetItem( StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]), result);


			Protocol::FromServer::AcquireItem acquireitem;
			acquireitem.ItemId = result.Info->Hash;
			acquireitem.Bag = static_cast<uint16>(StringUtil::To<int>(parameters[0]) );
			acquireitem.Pos = static_cast<uint16>(StringUtil::To<int>(parameters[1]) );
			acquireitem.StackedCount = result.StackedCount;
			acquireitem.instanceEx = result.InstanceEx;

			user->Send( acquireitem );
		}
		
		const Database::Info::ItemInfo* acquireItemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve( dailyItemInfo->ItemHash );	
		if( (acquireItemInfo->ItemType != Database::Info::ItemInfo::Type::EQUIPMENT) && (acquireItemInfo->ItemType != Database::Info::ItemInfo::Type::UNIDENTIFIED) ) {			
			uint32 slotCount = (dailyItemInfo->SlotCount/acquireItemInfo->MaxCnt);
			for( size_t i =0; i < slotCount; ++i )
			{
				user->ItemAdd(dailyItemInfo->ItemHash,0,acquireItemInfo->MaxCnt,0,false);				
			}
			if( (dailyItemInfo->SlotCount % acquireItemInfo->MaxCnt) != 0 ) {
				user->ItemAdd(dailyItemInfo->ItemHash,0,(dailyItemInfo->SlotCount % acquireItemInfo->MaxCnt),0,false);				
			}
		}else {
			size_t max = dailyItemInfo->SlotCount;
			for( size_t i =0; i < max; ++i )
			{
				user->ItemAdd(dailyItemInfo->ItemHash,0,1,0,false);
			}
		}
		
	}

	/*
	bool DailyItem::IsHaveTodayDailyItem(const AllM::DateTime::Date& connectDate,Database::Enchant::NormalBitfields instance) const
	{
		//expire date�� �˻� �� �Ŀ�.
		if( (connectDate.GetYear() < instance.ExpiredYear) 
			&& (connectDate.GetMonth() < instance.ExpiredMonth) 
			&& (connectDate.GetDay() < instance.ExpiredDay) ) 
		{			
			DailyItemLastAcquiredDate	date;
			date.Year = instance.CashEnchant1;
			date.Month = (instance.CashEnchant2 & 480) >> 5;
			date.Day = instance.CashEnchant2 & 31;
			if( (static_cast<uint16>(date.Year) <= connectDate.GetYear()) 
				&& (static_cast<uint16>(date.Month) <= connectDate.GetMonth()) 
				&& (static_cast<uint16>(date.Day) < connectDate.GetDay()) ) 
			{
				return true;
			}
		}		
		return false;
	}
	*/
	/********************************************************************************* DailyItem */


	/* ExtendExpireItemDate ********************************************************************************/
	ExtendExpireItemDate::ExtendExpireItemDate(UserSharedPtr user, Items& items)
		: user(user), items(items), useable(false)
	{
	}

	const wchar_t* ExtendExpireItemDate::GetTypeName() const
	{
		return L"ExtendExpireItemDate";
	}

	uint16 ExtendExpireItemDate::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{	
		useable = false;

		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		if (parameters.size() != 4)
			return 0;
			//throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for ExtendExpireItemDate" )) );

		const Common::ItemEx* item = items.GetItem(StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]));

		uint32 extendableItemHash(GetNoLogicStateValue(info, L"itemHash", 0));

		if(extendableItemHash != 0 && extendableItemHash != item->Info->Hash)
			return 0;

		if(Database::Enchant::IsExpired(item->Info, item->InstanceEx) == false)
			return 0;

		if(!(item->Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::EXISTWHENEXPIRED))
			return 0;

		if( StringUtil::ToInt( parameters[0] ) < 1 )
		{
			return 0;
		}

		useable = true;

		return 1;
	}

	void ExtendExpireItemDate::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(useable == false)
			return;

		uint32 bag = StringUtil::ToInt(parameters[0]);
		uint32 position = StringUtil::ToInt(parameters[1]);

		const Common::ItemEx* item = items.GetItem(bag, position);

		InstanceEx i = item->InstanceEx;
		std::wstring extendDayValue = GetNoLogicStateValue<std::wstring>(info, L"extendDay", L"0");
		if( extendDayValue == L"UNLIMITED" )
		{
			uint32 extendDay = StringUtil::To<uint32>( extendDayValue );	
			i.MakeUnlimitedPeriod();
		}
		else
		{
			uint32 extendDay = StringUtil::To<uint32>( extendDayValue );
			i.ExtensionExpiredDay(extendDay);
		}
		items.ChangeItemInstance(bag, position, i);

		/* notice to client */
		Protocol::FromServer::UpdateItem packet;
		packet.Position.Bag = bag;
		packet.Position.Position = position;
		packet.ItemHash = item->Info->Hash;
		packet.instanceEx = i;

		user->Send(packet);

		bag = StringUtil::ToInt(parameters[2]);
		position = StringUtil::ToInt(parameters[3]);


		/*Logger().Write(IAdmLogger::NormalLogger, "pet-extend", user->GetName(), Http::Logger::Parameter()
							<< item->Serial << static_cast<int64>(item->InstanceEx.Instance) << info->Hash );*/

		user->DropItems(bag, position, 1);
	}
	/******************************************************************************** ExtendExpireItemDate */

	/* ForceItemExpiration ********************************************************************************/
	ForceItemExpiration::ForceItemExpiration(UserSharedPtr user, Items& items)
		: user(user), items(items), useable(false)
	{
	}

	const wchar_t* ForceItemExpiration::GetTypeName() const
	{
		return L"ForceItemExpiration";
	}

	uint16 ForceItemExpiration::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{	
		useable = false;

		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		if (parameters.size() != 4)
			return 0;
			//throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for ExtendExpireItemDate" )) );

		const Common::ItemEx* item = items.GetItem(StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]));

		uint32 extendableItemHash(GetNoLogicStateValue(info, L"itemHash", 0));

		if(extendableItemHash != 0 && extendableItemHash != item->Info->Hash)
			return 0;

		if(Database::Enchant::IsExpired(item->Info, item->InstanceEx)) //
			return 0;

		if(!(item->Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::EXISTWHENEXPIRED))
			return 0;

		useable = true;

		return 1;
	}

	void ForceItemExpiration::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(useable == false)
			return;

		uint32 bag = StringUtil::ToInt(parameters[0]);
		uint32 position = StringUtil::ToInt(parameters[1]);

		const Common::ItemEx* item = items.GetItem(bag, position);

		InstanceEx i = item->InstanceEx;
		i.ForceExpiration();
		items.ChangeItemInstance(bag, position, i);

		/* notice to client */
		Protocol::FromServer::UpdateItem packet;
		packet.Position.Bag = bag;
		packet.Position.Position = position;
		packet.ItemHash = item->Info->Hash;
		packet.instanceEx = item->InstanceEx;

		user->Send(packet);

		bag = StringUtil::ToInt(parameters[2]);
		position = StringUtil::ToInt(parameters[3]);


		/*Logger().Write(IAdmLogger::NormalLogger, "forceItemExpiration", user->GetName(), Http::Logger::Parameter()
							<< item->Serial << static_cast<int64>(item->InstanceEx.Instance) << item->InstanceEx.ExpireDate.ToString() << info->Hash );*/

		user->DropItems(bag, position, 1);
	}
	/******************************************************************************** ForceItemExpiration */


	/* PetItem ***********************************************************************************/
	PetItem::PetItem(UserSharedPtr user, Items& items)
		: user( user ), items( items )
	{
	}

	const wchar_t* PetItem::GetTypeName() const
	{
		return L"PetItem";
	}

	uint16 PetItem::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		return 0;
	}

	void PetItem::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(user->GetCharacterStateFlags().IsPetNotUsable == true)
			return;

		if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(info->Hash) == true)
		{
			if( parameters.size() < 1 ) return;

			String type = parameters[0];
			Logic::Player* player = user->GetPlayer();

			if( type == L"naming" )
			{
				if( parameters.size() < 4 ) return;

				String petName = parameters[1];
				const Common::ItemEx *item = items.GetItem( StringUtil::ToInt( parameters[2] ), StringUtil::ToInt( parameters[3] ) );

				user->CreatePet(petName, item->Serial, item->Info->Hash);
			}
			else if( type == L"create" )
			{
				if(parameters.size() < 3)
					return;

				uint8 bag = StringUtil::ToInt(parameters[1]);
				uint8 position = StringUtil::ToInt(parameters[2]);

				const Common::ItemEx *item = items.GetItem(bag, position);
				if(Database::Enchant::IsExpired(item->Info, item->InstanceEx) && bag != 0)
					return;

				user->SummonPet(item->Serial, bag, position);
			}
			else if( type == L"destroy" )
			{
				if( parameters.size() < 3 ) return;

				user->UnsummonPet();
			}			
		}

		return;
	}
	/*********************************************************************************** PetItem */

	/* PetFood ***********************************************************************************/
	PetFood::PetFood(UserSharedPtr user, Items& items)
		: user( user ), items( items )
	{
	}

	const wchar_t* PetFood::GetTypeName() const
	{
		return L"PetFood";
	}

	uint16 PetFood::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		return 0;
	}

	void PetFood::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if( parameters.size() >= 6 && parameters[0] == L"Feeding" )
		{
			Database::Info::ActionInfo::StateList::const_iterator i
				= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

			const Common::ItemEx *item = items.GetItem( StringUtil::ToInt( parameters[1] ), StringUtil::ToInt( parameters[2] ) );
			if (item == NULL) 
			{
				LoggerInstance().Exception("invalid pet item position to feed petfood - bag[{}], position[{}]", StringUtil::ToInt(parameters[1]), StringUtil::ToInt(parameters[2]));
				return;
			}

			int foodCount = 0;
			ItemPosition foodPos;

			for( uint8 index = 3; index < parameters.size(); index += 3 )
			{
				foodPos.Bag      = StringUtil::ToInt( parameters[index] );
				foodPos.Position = StringUtil::ToInt( parameters[index + 1] );			
				foodCount = StringUtil::ToInt( parameters[index + 2] );

				std::map<String, String>::const_iterator keyValue = i->params.find( L"Amount" );
				if (keyValue==i->params.end())
				{
					LoggerInstance().Error( L"item ({}:{}) must have 'Amount' parameter to UseItemEx", info->Hash, info->Id.c_str() );
					return;
				}

				uint32 foodAmount = StringUtil::ToInt( keyValue->second );

				bool overFeed = false;

				keyValue = i->params.find( L"OverFeed" );
				if ( keyValue != i->params.end() )
				{
					overFeed = StringUtil::ToInt( keyValue->second ) > 0 ? true : false;
				}
				
				user->PetFeed( item->Serial, foodAmount, foodCount, overFeed );

				// Drop Item ///////////////////////////////////////////////////////////////////////////
				user->DropItems( foodPos.Bag, foodPos.Position, foodCount );
				/////////////////////////////////////////////////////////////////////////// Drop Item //
			}
		}
	}

	/* PetLevelDown ***********************************************************************************/
	PetLevelDown::PetLevelDown(UserSharedPtr user, Items& items)
		: user(user), items(items), useable(false)
	{
	}

	const wchar_t* PetLevelDown::GetTypeName() const
	{
		return L"PetLevelDown";
	}

	uint16 PetLevelDown::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		useable = false;

		Database::Info::ActionInfo::StateList::const_iterator i
			= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

		if (parameters.size() != 2)
			return 0;
			//throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for GetConsumableCount - wrong param size PetLevelDown" )) );

		const Common::ItemEx* petItem = items.GetItem(StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]));

		if(Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(petItem->Info->Hash) == false)
			throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for GetConsumableCount - invalid position PetLevelDown");

		/// �Ⱓ�� ����� ��������� ���ٿ���� ��ġ�� ���٤Ф�
		if(Database::Enchant::IsExpired(petItem->Info, petItem->InstanceEx) == true)
			return 0;

		if(user->GetPlayer() != NULL && user->GetPlayer()->IsRarePet(petItem->Serial) == true)
			return 0;

		bool onlyMaxLevel(GetNoLogicStateValue(info, L"onlyMaxLevel", 0));

		if(onlyMaxLevel == true && user->GetPlayer() != NULL && user->GetPlayer()->IsMaxPetLevel(petItem->Serial) == false)
			return 0;

		useable = true;

		return 1;
	}

	void PetLevelDown::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(useable == false)
			return;

		const Common::ItemEx* petItem = items.GetItem(StringUtil::ToInt(parameters[0]), StringUtil::ToInt(parameters[1]));

		uint32 downLevel(GetNoLogicStateValue(info, L"downLevel", 1));
		bool   onlyMaxLevel(GetNoLogicStateValue(info, L"onlyMaxLevel", 0));

		user->PetLevelDecrease(petItem->Serial/*Pet Serial*/, downLevel, onlyMaxLevel);
	}
	/*********************************************************************************** PetLevelDown */

	/* PetSchoolTicket ********************************************************************************/
	PetSchoolTicket::PetSchoolTicket(UserSharedPtr user, Items& items)
		: user(user), items(items), useable(false)
	{
	}

	const wchar_t* PetSchoolTicket::GetTypeName() const
	{
		return L"PetSchoolTicket";
	}

	uint16 PetSchoolTicket::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		useable = false;

		if(parameters.size() != 3)
			return 0;

		if( parameters[0] != L"PetSchoolCommand" )
			return 0;

		const Common::ItemEx* petItem = items.GetItem(StringUtil::ToInt(parameters[1]), StringUtil::ToInt(parameters[2]));

		if(Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(petItem->Info->Hash) == false)
			throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for GetConsumableCount - invalid position PetLevelDown"  );

		if(Database::Enchant::IsExpired(petItem->Info, petItem->InstanceEx) == true)
			return 0;

		if(user->GetPlayer() != NULL && user->GetPlayer()->IsMaxPetLevel(petItem->Serial) == true)
			return 0;

		useable = true;

		return 1;
	}

	void PetSchoolTicket::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(useable == false)
			return;

		const Common::ItemEx* petItem = items.GetItem( StringUtil::ToInt( parameters[1] ), StringUtil::ToInt( parameters[2] ) );

		float period( GetNoLogicStateValue( info, L"Period", 0.0f ) );
		float expFactor( GetNoLogicStateValue( info, L"ExpFactor", 1.0f ) );

		user->TakeCarePetBySchool( petItem->Info->Hash, petItem->Serial
								, petItem->InstanceEx.Instance, petItem->StackedCount // This conversion should defenetly be avoided but since enchant has no expire it doesnt matter anyways.
			                    , (uint32)period, expFactor, StringUtil::ToInt( parameters[1] ), StringUtil::ToInt( parameters[2] ), info->Hash );
	}
	/******************************************************************************** PetSchoolTicket */

	/* PetRenaming ********************************************************************************/
	PetRenaming::PetRenaming(UserSharedPtr user, Items& items)
		: user(user), items(items), useable(false)
	{
	}

	const wchar_t* PetRenaming::GetTypeName() const
	{
		return L"PetRenaming";
	}

	uint16 PetRenaming::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		useable = false;

		if(parameters.size() != 3)
			return 0;

		const Common::ItemEx* petItem = items.GetItem(StringUtil::ToInt(parameters[1]), StringUtil::ToInt(parameters[2]));

		if(Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(petItem->Info->Hash) == false)
			throw InvalidRuntimeParameterException( L"invalid UseItemEx parameter for GetConsumableCount - invalid position PetLevelDown");

		if(Database::Enchant::IsExpired(petItem->Info, petItem->InstanceEx) == true)
			return 0;

		if(user->GetPetDatas().GetPetData(petItem->Serial)->Appear == false)
			return 0;

		useable = true;

		return 1;
	}

	void PetRenaming::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(useable == false)
			return;

		const Common::ItemEx* petItem = items.GetItem(StringUtil::ToInt(parameters[1]), StringUtil::ToInt(parameters[2]));

		user->PetRename(petItem->Serial, parameters[0]);
	}
	/******************************************************************************** PetRenaming */

	/* CharacterRenaming ********************************************************************************/
	CharacterRenaming::CharacterRenaming(UserSharedPtr user, Items& items)
		: user(user), items(items),itemPosition(XRated::ItemPosition::Invalid)
	{
	}

	const wchar_t* CharacterRenaming::GetTypeName() const
	{
		return L"CharacRenaming";
	}

	uint16 CharacterRenaming::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		return 0;
	}

	void CharacterRenaming::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		static DateTime prevTime;
		static std::wstring prevCharacterName;
		if( prevTime.IsValid() == true && prevCharacterName == user->GetName())
		{
			TimeSpan now, prev;
			now.SetTimeSpan( DateTime::Now() );
			prev.SetTimeSpan( prevTime );
			TimeSpan delta = now - prev;
			
			LoggerInstance().Info(L"delta.GetTotalSeconds() = {0}", delta.GetTotalSeconds() );

			if( delta.GetTotalSeconds() < 10 )
			{
				return;
			}
		}
		
		itemPosition.Bag = StringUtil::ToInt( parameters[1] );
		itemPosition.Position = StringUtil::ToInt( parameters[2] );
		Net::Api request("ReserveRenameCharacter");
		request << user->GetSerial() << parameters[0];
		request.GetAsync(this,&CharacterRenaming::Renamed, user->shared_from_this());

		prevTime = DateTime::Now();
		prevCharacterName = user->GetName();
	}
	void CharacterRenaming::Renamed(const UserSharedPtr& user, const Net::Answer& answer)
	{
		//CharacterRenaming::Renamed
		if (answer.errorCode == 0) // ok
		{
			std::wstring oldName = answer.resultObject.at("oldName").get<std::wstring>();
			std::wstring newName = answer.resultObject.at("newName").get<std::wstring>();
			Protocol::FromServer::Rename packet;
			packet.TargetType = Protocol::FromServer::Rename::Types::Character;
			packet.Result = Protocol::FromServer::Rename::Results::OK;
			user->Send(packet);
			items.DropItems(itemPosition.Bag, itemPosition.Position, 1, true);

			//Logger().Write(IAdmLogger::NormalLogger, "rename-reserve", oldName, Http::Logger::Parameter() << newName);
		}
		else
		{
			Protocol::FromServer::Rename packet;
			packet.TargetType = Protocol::FromServer::Rename::Types::Character;
			packet.Result = (Protocol::FromServer::Rename::Results)answer.errorCode;
			user->Send(packet);
		}
	}
	/******************************************************************************** CharacterRenaming */

	/* AddLicense ********************************************************************************/
	AddLicense::AddLicense(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* AddLicense::GetTypeName() const
	{
		return L"AddLicense";
	}

	uint16 AddLicense::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		stageLicense.StageGroupHash = 0;

		if (parameters.size() != 0)
			throw InvalidRuntimeParameterException(((L"invalid UseItemEx parameter for GetConsumableCount - wrong param size AddLicense")));

		XRated::StageLicense license;
		license.StageGroupHash = GetNoLogicStateValue(info, L"StageGroupHash", (uint32)0);
		int stageLevel = GetNoLogicStateValue(info, L"StageLevel", -1);

		if(license.StageGroupHash == 0)
			return 0;

		Database::Info::StageGroup* stageGroup = Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(license.StageGroupHash);
		if(stageGroup == NULL)
			return 0;

		const std::vector<XRated::StageLicense>& stageLicenses = user->GetStageLicenses();
		std::vector<XRated::StageLicense>::const_iterator itr = std::find_if(stageLicenses.begin(), stageLicenses.end(),
																			XRated::StageLocation::FinderOnlyStageGroupHash(license.StageGroupHash));

		if(stageLevel == -1)
		{
			if(itr == stageLicenses.end())
			{
				const Database::Info::StageLicenseProvider::StageLicenseList *licenseList = Database::DatabaseInstance().InfoCollections.LicenseProvider.GetStageLicenseList(user->GetLevel(), user->GetRebirthCount());
				if(!licenseList) return 0;

				std::vector<XRated::StageLicense>::const_iterator licenseItr = std::find_if(licenseList->begin(), licenseList->end(),
																						XRated::StageLocation::FinderOnlyStageGroupHash(license.StageGroupHash));
				if(licenseItr == licenseList->end()) return 0;
				else
				{
					stageLevel = licenseItr->Level + 1;
				}
			}
			else
			{
				const Database::Info::StageLicenseProvider::StageLicenseList *licenseList = Database::DatabaseInstance().InfoCollections.LicenseProvider.GetStageLicenseList(user->GetLevel(), user->GetRebirthCount());
				if(licenseList)
				{
					std::vector<XRated::StageLicense>::const_iterator licenseItr = std::find_if(licenseList->begin(), licenseList->end(),
																							XRated::StageLocation::FinderOnlyStageGroupHash(license.StageGroupHash));
					if(licenseItr != licenseList->end())
					{
						if(itr->Level < licenseItr->Level)
							stageLevel = licenseItr->Level + 1;
						else
							stageLevel = itr->Level + 1;
					}
					else
						stageLevel = itr->Level + 1;
				}
				else
					stageLevel = itr->Level + 1;
			}

			if ( stageGroup->EpisodeInterval != 0 && (stageLevel % stageGroup->EpisodeInterval == 0) )
				return 0;
			if ( stageLevel >= stageGroup->Stages.size() )
				return 0;

			stageLicense = license;
			stageLicense.Level = static_cast<uint16>(stageLevel);
		}
		else
		{
			if(itr == stageLicenses.end() || itr->Level < stageLevel)
				stageLicense.Level = static_cast<uint16>(stageLevel);
			else
				return 0;

			stageLicense.StageGroupHash = stageGroup->Hash;
		}

		return 1;
	}

	void AddLicense::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(stageLicense.StageGroupHash == 0)
			return;

		if(user->AddStageLicense(stageLicense, true) == true)
		{
			//user->RequestUpdateDBStageLicense(stageLicense);

			Protocol::FromServer::AcquireLicense packet;
			packet.serial = user->GetSerial();
			packet.stageLicense = stageLicense;
			packet.sharedOtherPlayers = 0; // 3.1 by ultimate
			user->Send(packet);
		}
		else
			LoggerInstance().Info("AddLicense::Use() - user->AddStageLicense Failed User:{} StageGroupHash:{} Level:{}",
					user->GetSerial(), stageLicense.StageGroupHash, stageLicense.Level);

	}
	/******************************************************************************** AddLicense */

	/* AddItemToGuildShop ********************************************************************************/
	AddItemToGuildShop::AddItemToGuildShop(UserSharedPtr& user, GuildManager& guild)
		: user(user), guild(guild)
	{
	}
	const wchar_t* AddItemToGuildShop::GetTypeName() const
	{
		return L"AddItemToGuildShop";
	}
	uint16 AddItemToGuildShop::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		//validation
		uint32 itemHash = GetNoLogicStateValue(info, L"ItemHash", 0);
		uint8 usableGuildLevel = GetNoLogicStateValue(info, L"GuildLevel", 0);		
		if( itemHash == 0 ) { 
			throw InvalidRuntimeParameterException( fmt::format(L"AddItemToGuildShop : have not state : itemHash : {}",info->Hash).c_str() );
		}
		//check joined guild
		if( user->IsPartOfGuild() == false ) {	
			throw InvalidRuntimeParameterException(L"AddItemToGuildShop Use Failed : not joined guild" );
		}
		if( user->GetGuildInfo().GuildLevel < usableGuildLevel ) {
			throw InvalidRuntimeParameterException( L"AddItemToGuildShop Use Failed : usable guild level");
		}
		if( info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME ) {
			return 1;
		}
		return 0;
	}
	void AddItemToGuildShop::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		//request guild manager
		uint32 itemHash = GetNoLogicStateValue(info, L"ItemHash", 0);
		uint8 usableGuildLevel = GetNoLogicStateValue(info, L"GuildLevel", 0);
		int day = GetNoLogicStateValue(info, L"Day", 0);
		guild.RequestAddItemToGuildShop( user, itemHash, usableGuildLevel, day );
	}
	/******************************************************************************** AddItemToGuildShop */

	Rebirth::Rebirth(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* Rebirth::GetTypeName() const
	{
		return L"Rebirth";
	}

	uint16 Rebirth::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		useable = false;

		if(user->GetRoom()->GetRoomKind() != Common::SQUARE)
		{
			throw InvalidRuntimeParameterException( ((L"Rebirth Failed : not square")) );
		}

		if(DateTime::Now() > user->GetLastRebirthDate())
		{
			int month = static_cast<int>(DateTime::Now().GetDate().GetMonth()) - static_cast<int>(user->GetLastRebirthDate().GetDate().GetMonth());
			int year = static_cast<int>(DateTime::Now().GetDate().GetYear()) - static_cast<int>(user->GetLastRebirthDate().GetDate().GetYear());

			if(month < 0)
				month += 12;
			month += 12 * year;

			int delayMonth = GetNoLogicStateValue(info, L"DelayMonth", 2);

			useable = month >= delayMonth;
		}
		if( user->IsFishing() == true ) {
			throw InvalidRuntimeParameterException(fmt::format(L"Rebirth Failed, now fishing : {}",info->Hash).c_str());
		}

		if(useable == false)
		{
			Protocol::FromServer::Rebirth packet;
			packet.Result = Protocol::FromServer::Rebirth::Results::RebirthFailedNeedMoreDay;
			packet.PlayerSerial = 0;
			packet.Level = 0;
			packet.StoredLevel = 0;
			packet.RebirthCount = 0;
			packet.StoredSkillPoint = 0;
			packet.LastRebirthDateTime = DateTime::Now();
			packet.UpdatedLicense.clear();
			user->Send(packet);
			throw InvalidRuntimeParameterException(L"Rebirth Failed, Need more day", false );

		}

		return 0;
	}


    void Rebirth::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if( parameters.size() < 2 )
		{
			throw InvalidRuntimeParameterException(fmt::format(L"Rebirth Failed, parameter broken : {}",info->Hash).c_str());
		}

		user->RebirthCharacter(XRated::ItemPosition(StringUtil::ToInt( parameters[0] ), StringUtil::ToInt( parameters[1] ) ) );
	}


	/* AddLife ***********************************************************************************/
	AddLife::AddLife(XRated::Logic::Player*& player, std::shared_ptr<IUserRoom>& room, XRated::CharacterStateFlags& characterStateFlags)
		: player(player), room(room), characterStateFlags(characterStateFlags)
	{
	}

	const wchar_t* AddLife::GetTypeName() const
	{
		return L"AddLife";
	}

	uint16 AddLife::GetConsumableCount(const Database::Info::ItemInfo* /*info*/, const std::vector<String>& /*parameters*/) const
	{
		return 0; // passive item should be always 0
	}

	void AddLife::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	{
		std::cout << "AddLife::Use" << std::endl;
		int life(GetNoLogicStateValue(info, L"Life", 1));
		bool pvpUse(GetNoLogicStateValue(info, L"PvpUse", 0)?true:false);
		bool stackable(GetNoLogicStateValue(info, L"Stackable", 0)?true:false);

		const Database::Info::StageGroup* stageGroup = room->GetCurrentStageGroupInfo();
		if( stageGroup != NULL && stageGroup->Stages.size() > room->GetCurrentStage().Level )
		{
			Database::Info::StageInfo* stageInfo = stageGroup->Stages.at( room->GetCurrentStage().Level );
			if( stageInfo != NULL && stageInfo->isReviveAvailable == false )
			{
				return; // ��Ȱ�� �Ұ����� ����������.
			} else {
				/*if ( room->GetReviveLimitCount() > -1 
					&& room->GetReviveCount() >= 
					room->GetReviveLimitCount() ){
					return;
				}*/
			}
		}		

		XRated::PlayerData& data=player->GetPlayerData();
		if ( (room->GetRoomKind()==Common::PVP && pvpUse) || (room->GetRoomKind()==Common::STAGE) )
		{
			if ( stackable || characterStateFlags.IsPcRoom==false )
			{
				if ( (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::PASSIVE)
					&& room->GetCurrentStageGroupInfo()->SharedLife
					&& room->GetCurrentStage().Level>0 )
				{
					/* addlife should not be affected in shared life stage except first stage of the stage group */
				}
				else
				{
					data.Life=static_cast<uint16>(data.Life+life);
				}
			}
		}
	}
	/*********************************************************************************** AddLife */

	/* ExpFactor ***********************************************************************************/
	ExpFactor::ExpFactor(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* ExpFactor::GetTypeName() const
	{
		return L"ExpFactor";
	}

	uint16 ExpFactor::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/) const
	{
		//validate category.
		int category(GetNoLogicStateValue(info, L"Category", 0));
		if( (category < XRated::Constants::ExpFactorCategoryFromItem::CategoryNone)
			|| (category >= XRated::Constants::ExpFactorCategoryFromItem::MaxCategory) )
		{			
			throw InvalidRuntimeParameterException(fmt::format(L"Wrong ExpFactor NoLogicItem : {}",info->Hash).c_str());
		}
		return 0; // passive item should be always 0
	}

	void ExpFactor::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	{
		float factor(GetNoLogicStateValue(info, L"Factor", 1.0f));
		int category(GetNoLogicStateValue(info, L"Category", 0));		
		user->SetExpFactorFromItem( static_cast<XRated::Constants::ExpFactorCategoryFromItem>(category), factor );

		// old version 2008-09-05 by kwind
		//bool stackable(GetNoLogicStateValue(info, L"Stackable", 0)?true:false);
		//XRated::PlayerData& data=user->GetPlayer()->GetPlayerData(); // data.expFactor already PC room accepted
		//data.expFactor*=factor;
		//if (stackable==false) // make cancel less effect 
		//{			
		//	data.expFactor/=((factor<user->GetExtraExpFactor())?factor:user->GetExtraExpFactor());			
		//}
	}
	/*********************************************************************************** ExpFactor */

	/* PetAppetiteControl ***********************************************************************************/
	//PetAppetiteControl::PetAppetiteControl(XRated::Logic::Player*& player)
	//	: player(player)
	//{
	//}

	//const wchar_t* PetAppetiteControl::GetTypeName() const
	//{
	//	return L"PetAppetiteControl";
	//}

	//uint16 PetAppetiteControl::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/) const
	//{
	//	return 0; // passive item should be always 0
	//}

	//void PetAppetiteControl::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	//{
	//	XRated::PlayerData& data = player->GetPlayerData();

	//	//data.fullFactor = GetNoLogicStateValue(info, L"FullFactor", XRated::Pet::TiredFactorRelatedWithFull::Full);
	//	//data.goodFactor = GetNoLogicStateValue(info, L"GoodFactor", XRated::Pet::TiredFactorRelatedWithFull::Good);
	//	//data.sosoFactor = GetNoLogicStateValue(info, L"SosoFactor", XRated::Pet::TiredFactorRelatedWithFull::Soso_NotGood);
	//	//data.hungryFactor = GetNoLogicStateValue(info, L"HungryFactor", XRated::Pet::TiredFactorRelatedWithFull::Hungry_Starving);
	//}
	/*********************************************************************************** PetAppetiteControl */
	
	/* GuildLevelUp *********************************************************************************/
	GuildLevelUp::GuildLevelUp(UserSharedPtr user, GuildManager& pGuild)
		: user(user), guild(pGuild)
	{
	}

	const wchar_t* GuildLevelUp::GetTypeName() const
	{
		return L"GuildLevelUp";
	}

	uint16 GuildLevelUp::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/) const
	{
		//if (parameters.size() < 1)
		//	throw InvalidRuntimeParameterException( L"not enough parameter" )) );

		throw InvalidRuntimeParameterException( L"Guild LeveUp Item : not support this item from 2008-09-10" );
		return 0;
		/*
		int level(GetNoLogicStateValue(info, L"GuildLevel", -1));
		if ( level == -1 )
			throw InvalidRuntimeParameterException( L"guildLevel is wrong" )) );

		if ( user->IsJoined() == false )
		{
			return 111;
		}
		else if ( user->GetGuildInfo().MasterName != user->GetName() )
		{
			Protocol::FromServer::AllMGuild::LevelUp response;
			response.Result = Protocol::FromServer::AllMGuild::LevelUp::Results::HaveNoPermission;
			user->Send(response);
			return 111;
		}
		else if ( user->GetGuildInfo().GuildLevel != level-1 || level<0 || level>5)
		{
			Protocol::FromServer::AllMGuild::LevelUp response;
			response.Result = Protocol::FromServer::AllMGuild::LevelUp::Results::InvalidLevel;
			user->Send(response);
			return 111;
		}
		
		return 1;
		*/
	}

	void GuildLevelUp::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	{
		//int level(GetNoLogicStateValue(info, L"GuildLevel", -1));
		//if ( level == -1 )
		//	throw InvalidRuntimeParameterException( L"guildLevel is wrong" )) );
		
		//guild.GuildLevelUp( &user, level );
	}	
	
	
	/********************************************************************************* GuildLevelUp */


	/*GuildPointUp ***********************************************************************************/
	const wchar_t* GuildPointUp::GetTypeName() const
	{
		return L"GuildPointUp";
	}
	GuildPointUp::GuildPointUp(UserSharedPtr& user, GuildManager& pGuild)
		: user(user), guild(pGuild)
	{
	}
	uint16 GuildPointUp::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{		
		if ( user->IsPartOfGuild() == false )
		{
			throw InvalidRuntimeParameterException( L"guildPoint up failed, not joined guild");
		}
		if( parameters.size() < 1 ) {
			return 1;
		}
		return StringUtil::To<int>(parameters[0]);
	}
	void GuildPointUp::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if( parameters.size() < 1 ) {
			guild.GuildPointUp( user, 1 );
		}else {
			guild.GuildPointUp( user, StringUtil::To<int>(parameters[0]) );
		}
	}	
	/********************************************************************************* GuildPointUp */

	/*GuildExpUp ***********************************************************************************/
	const wchar_t* GuildExpUp::GetTypeName() const
	{
		return L"GuildExpUp";
	}
	GuildExpUp::GuildExpUp(UserSharedPtr& user, GuildManager& pGuild)
		: user(user), guild(pGuild)
	{
	}
	uint16 GuildExpUp::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{		
		if ( user->IsPartOfGuild() == false )
		{
			throw InvalidRuntimeParameterException( L"guildExp up failed, not joined guild" );
		}
		if( parameters.size() < 1 ) {
			return 1;
		}
		return StringUtil::To<int>(parameters[0]);
	}
	void GuildExpUp::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if( parameters.size() < 1 ) {
			guild.GuildExpUp( user, 1 );
		}else {
			guild.GuildExpUp( user, StringUtil::To<int>(parameters[0]) );
		}
	}	
	/********************************************************************************* GuildExpUp */

	GuildRankingPointUp::GuildRankingPointUp(UserSharedPtr& user, GuildManager& guild) : user(user), guild(guild)
	{
	}

	/*GuildRankingPointUp ***********************************************************************************/
	const wchar_t* GuildRankingPointUp::GetTypeName() const
	{
		return L"GuildRankingPointUp";
	}
	uint16 GuildRankingPointUp::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{		
		if ( user->IsPartOfGuild() == false )
		{
			throw InvalidRuntimeParameterException( L"guildExp up failed, not joined guild");
		}
		if( parameters.size() < 1 ) {
			return 1;
		}
		return StringUtil::To<int>(parameters[0]);
	}
	void GuildRankingPointUp::Use(const Database::Info::ItemInfo* info, const std::vector<String>& )
	{
		guild.IncreaseGuildRankingPoint( user, GetNoLogicStateValue( info , L"Point", 1 ) );
	}	
	/********************************************************************************* GuildRankingPointUp */

	/* AddSkillPoint *********************************************************************************/
	SkillPointPlus::SkillPointPlus(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* SkillPointPlus::GetTypeName() const
	{
		return L"SkillPointPlus";
	}

	uint16 SkillPointPlus::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/) const
	{
		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void SkillPointPlus::Use(const Database::Info::ItemInfo* info, const std::vector<String>& /*parameters*/)
	{
		uint16 point(GetNoLogicStateValue(info, L"value", 0));
		if( point != 0 ) {
			user->AddSkillPointPlus(point);
		}
	}
	/********************************************************************************* AddSkillPoint */

	/* EventStage *********************************************************************************/
	EventStage::EventStage(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* EventStage::GetTypeName() const
	{
		return L"EventStage";
	}

	uint16 EventStage::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void EventStage::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		uint32 stageGroupHash = GetNoLogicStateValue(info, L"StageGroupHash", 0);
		uint16 accessLevel = GetNoLogicStateValue(info, L"AccessLevel", 0);

		/*InputMgr::GetGeneralController().RequestTo( Service::SquareService, L"userPopStage",
			StringUtil::Format(L"{} {},{}", user->GetName().c_str(), stageGroupHash, accessLevel) );

		InputMgr::GetGeneralController().RequestTo( Service::StageService, L"userPopStage",
			StringUtil::Format(L"{} {},{}", user->GetName().c_str(), stageGroupHash, accessLevel) );*/
	}
	/********************************************************************************* EventStage */

	/* EventDungeonStage *********************************************************************************/
	EventDungeonStage::EventDungeonStage(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* EventDungeonStage::GetTypeName() const
	{
		return L"EventDungeonStage";
	}

	uint16 EventDungeonStage::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void EventDungeonStage::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		uint32 stageGroupHash = GetNoLogicStateValue(info, L"StageGroupHash", 0);

		/*InputMgr::GetGeneralController().RequestTo( Service::SquareService, L"userPopStage",
			StringUtil::Format(L"{} {},{}", user->GetName().c_str(), stageGroupHash, 0) );

		InputMgr::GetGeneralController().RequestTo( Service::StageService, L"userPopStage",
			StringUtil::Format(L"{} {},{}", user->GetName().c_str(), stageGroupHash, 0) );*/
	}
	/********************************************************************************* EventDungeonStage */

	/* BookItem ***********************************************************************************/
	BookItem::BookItem( UserSharedPtr user )
		: user( user )
	{
	}

	const wchar_t* BookItem::GetTypeName() const
	{
		return L"BookItem";
	}

	uint16 BookItem::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void BookItem::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{

	}
	/**********************************************************************************  BookItem */

	/* BookPartItem *******************************************************************************/
	BookPartItem::BookPartItem( UserSharedPtr user )
		: user( user )
	{
	}

	const wchar_t* BookPartItem::GetTypeName() const
	{
		return L"BookPartItem";
	}

	uint16 BookPartItem::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CONSUME)?1:0;
	}

	void BookPartItem::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{
		// �������� �ϴ� �� ����.. Ŭ�󿡼� 
	}
	/******************************************************************************  BookPartItem */

	/* RestoreBelonging ***************************************************************************/
	RestoreBelonging::RestoreBelonging( UserSharedPtr user, Items& items )
		: user( user ), items( items )
	{
	}

	const wchar_t* RestoreBelonging::GetTypeName() const
	{
		return L"RestoreBelonging";
	}

	uint16 RestoreBelonging::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return 0;
	}

	void RestoreBelonging::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{
		if ( parameters.size() != 4 )
		{
			return;
		}

		user->DoRestoreBelonging( XRated::ItemPosition( StringUtil::ToInt( parameters[ 0 ] ), StringUtil::ToInt( parameters[ 1 ] ) )
							 , XRated::ItemPosition( StringUtil::ToInt( parameters[ 2 ] ), StringUtil::ToInt( parameters[ 3 ] ) ) );
	}
	/**************************************************************************  RestoreBelonging */

	/* Appraisal **********************************************************************************/
	Appraisal::Appraisal( UserSharedPtr user, Items& items )
		: user( user ), items( items )
	{
	}

	const wchar_t* Appraisal::GetTypeName() const
	{
		return L"Appraisal";
	}

	uint16 Appraisal::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return 0;
	}

	void Appraisal::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{
		if ( parameters.size() != 4 )
		{
			return;
		}

		user->DoAppraisal( XRated::ItemPosition( StringUtil::ToInt( parameters[ 0 ] ), StringUtil::ToInt( parameters[ 1 ] ) )
					  , XRated::ItemPosition( StringUtil::ToInt( parameters[ 2 ] ), StringUtil::ToInt( parameters[ 3 ] ) ) );
	}
	/*********************************************************************************  Appraisal */

	/* CashAppraisal **********************************************************************************/
	CashAppraisal::CashAppraisal( UserSharedPtr user, Items& items )
		: user( user ), items( items )
	{
	}

	const wchar_t* CashAppraisal::GetTypeName() const
	{
		return L"CashAppraisal";
	}

	uint16 CashAppraisal::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return 0;
	}

	void CashAppraisal::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{
		if ( parameters.size() != 4 )
		{
			return;
		}

		user->DoCashAppraisal( XRated::ItemPosition( StringUtil::ToInt( parameters[ 0 ] ), StringUtil::ToInt( parameters[ 1 ] ) )
					  , XRated::ItemPosition( StringUtil::ToInt( parameters[ 2 ] ), StringUtil::ToInt( parameters[ 3 ] ) ) );
	}
	/*********************************************************************************  CashAppraisal */

	/* RestoreAppraisal ***************************************************************************/
	RestoreAppraisal::RestoreAppraisal( UserSharedPtr user, Items& items )
		: user( user ), items( items )
	{
	}

	const wchar_t* RestoreAppraisal::GetTypeName() const
	{
		return L"RestoreAppraisal";
	}

	uint16 RestoreAppraisal::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return 0;
	}

	void RestoreAppraisal::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{
		if ( parameters.size() != 4 )
		{
			return;
		}

		user->DoRestoreAppraisal( XRated::ItemPosition( StringUtil::ToInt( parameters[ 0 ] ), StringUtil::ToInt( parameters[ 1 ] ) )
					         , XRated::ItemPosition( StringUtil::ToInt( parameters[ 2 ] ), StringUtil::ToInt( parameters[ 3 ] ) ) );
	}  
	/**************************************************************************  RestoreAppraisal */

	/* CashRestoreAppraisal ***********************************************************************/
	CashRestoreAppraisal::CashRestoreAppraisal( UserSharedPtr user, Items& items )
		: user( user ), items( items )
	{
	}

	const wchar_t* CashRestoreAppraisal::GetTypeName() const
	{
		return L"CashRestoreAppraisal";
	}

	uint16 CashRestoreAppraisal::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return 0;
	}

	void CashRestoreAppraisal::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{
		if ( parameters.size() != 4 )
		{
			return;
		}

		user->DoCashRestoreAppraisal( XRated::ItemPosition( StringUtil::ToInt( parameters[ 0 ] ), StringUtil::ToInt( parameters[ 1 ] ) )
								   , XRated::ItemPosition( StringUtil::ToInt( parameters[ 2 ] ), StringUtil::ToInt( parameters[ 3 ] ) ) );
	}  
	/**********************************************************************  CashRestoreAppraisal */

	/* Extract ************************************************************************************/
	Extract::Extract( UserSharedPtr user, Items& items )
		: user( user ), items( items )
	{
	}

	const wchar_t* Extract::GetTypeName() const
	{
		return L"Extract";
	}

	uint16 Extract::GetConsumableCount( const Database::Info::ItemInfo* info, const std::vector<String>& parameters ) const
	{
		return 0;
	}

	void Extract::Use( const Database::Info::ItemInfo* info, const std::vector<String>& parameters )
	{
		if ( parameters.size() != 4 )
		{
			return;
		}

		user->DoExtract( XRated::ItemPosition( StringUtil::ToInt( parameters[ 0 ] ), StringUtil::ToInt( parameters[ 1 ] ) )
					, XRated::ItemPosition( StringUtil::ToInt( parameters[ 2 ] ), StringUtil::ToInt( parameters[ 3 ] ) ) );
	}
	/***********************************************************************************  Extract */

	/* AddSkillLicense ********************************************************************************/
	AddSkillLicense::AddSkillLicense(UserSharedPtr user)
		: user(user)
	{
	}

	const wchar_t* AddSkillLicense::GetTypeName() const
	{
		return L"AddSkillLicense";
	}

	uint16 AddSkillLicense::GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const
	{
		skillGroup = NULL;

		if (parameters.size() != 0)
			throw InvalidRuntimeParameterException(L"invalid UseItemEx parameter for GetConsumableCount - wrong param size AddSkillLicense");

		uint32 skillGroupHash = GetNoLogicStateValue(info, L"SkillGroupHash", (uint32)0);
		uint32 classHash = XRated::Constants::GetClassHash( user->GetClassType() );
	
		Database::Info::SkillInfoList::SkillGroup* skill = Database::DatabaseInstance().InfoCollections.Skills.GetSkillGroupInfo(classHash, skillGroupHash);

		if( !skill || !skill->NeedLicense || user->HasSkillLicense(skillGroupHash) )
			return 0;

		skillGroup = skill;
		return 1;
	}

	void AddSkillLicense::Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters)
	{
		if(skillGroup == NULL)
			return;

		user->AddSkillLicense(skillGroup->Hash);
		{
			Protocol::FromServer::AcquireSkillLicense packet;
			packet.serial = user->GetSerial();
			packet.skillLicense = skillGroup->Hash;
			user->Send(packet);
		}
		//else
		//	ALLM_INFO((L"AddSkillLicense::Use() - user->AddSkillLicense Failed User : {} skillGroupHash :{}", user->GetName().c_str(), skillGroup->Hash ));

	}
	/******************************************************************************** AddLicense */


} } }