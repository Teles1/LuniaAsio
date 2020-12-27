#include "CommandQueue.h"


namespace Lunia { namespace XRated {	namespace Logic {

	CommandQueue::CommandQueue()
	{
		joinQueue.reserve(30);
		useQueue.reserve(30);
		petItemQueue.reserve(30);
		petFeedQueue.reserve(30);
		petLevelDecreaseQueue.reserve(30);
		presentToPetQueue.reserve(30);
		createPetQueue.reserve(30);
		petEquipQueue.reserve(30);		
		takeCarePetBySchoolQueue.reserve(30);
		takeOutCaredPetQueue.reserve(30);
		equipQueue.reserve(30);
		changeEquipToEquipQueue.reserve(30);
		//joinQueue.reserve(30);	/// by kpongky( 09.08.17 ) remove duplication
		createItemQueue.reserve(30);
		rebirthQueue.reserve(30);
		petRenamingQueue.reserve(30);
		petUnsummonQueue.reserve(30);
		orderQueue.reserve(30);

		tempJoinQueue.reserve(30);
		tempUseQueue.reserve(30);
		tempPetItemQueue.reserve(30);
		tempPetFeedQueue.reserve(30);
		tempPresentToPetQueue.reserve(30);
		tempPetLevelDecreaseQueue.reserve(30);
		tempCreatePetQueue.reserve(30);
		tempPetEquipQueue.reserve(30);		
		tempTakeCarePetBySchoolQueue.reserve(30);
		tempTakeOutCaredPetQueue.reserve(30);
		tempEquipQueue.reserve(30);
		tempChangeEquipToEquipQueue.reserve(30);
		tempCreateItemQueue.reserve(30);
		tempCmdQueue.reserve(30);
		tempRebirthQueue.reserve(30);
		tempPetRenamingQueue.reserve(30);
		tempPetUnsummonQueue.reserve(3);
		tempOrderQueue.reserve(30);
		tempSwapEquipmentQueue.reserve(30);
		tempCashSwapEquipmentQueue.reserve(30);
	}

	void CommandQueue::SwapQueue()
	{
		AutoLock locker(cmdQueueCS);

		ClearTempQueue();

		tempJoinQueue.swap(joinQueue);
		tempUseQueue.swap(useQueue);
		tempPetItemQueue.swap(petItemQueue);
		tempPetFeedQueue.swap(petFeedQueue);
		tempPetLevelDecreaseQueue.swap(petLevelDecreaseQueue);
		tempPresentToPetQueue.swap(presentToPetQueue);
		tempCreatePetQueue.swap(createPetQueue);
		tempPetEquipQueue.swap(petEquipQueue);		
		tempTakeCarePetBySchoolQueue.swap(takeCarePetBySchoolQueue);
		tempTakeOutCaredPetQueue.swap(takeOutCaredPetQueue);
		tempEquipQueue.swap(equipQueue);
		tempChangeEquipToEquipQueue.swap(changeEquipToEquipQueue);
		tempCreateItemQueue.swap(createItemQueue);
		tempCmdQueue.swap(cmdQueue);
		tempRebirthQueue.swap(rebirthQueue);
		tempPetRenamingQueue.swap(petRenamingQueue);
		tempPetUnsummonQueue.swap(petUnsummonQueue);
		tempOrderQueue.swap(orderQueue);
		tempSwapEquipmentQueue.swap( swapEquipmentQueue );
		tempCashSwapEquipmentQueue.swap( swapCashEquipmentQueue );

		joinIter = tempJoinQueue.begin();
		useIter = tempUseQueue.begin();
		petItemIter = tempPetItemQueue.begin();
		petFeedIter = tempPetFeedQueue.begin();
		presentToPetIter = tempPresentToPetQueue.begin();
		petLevelDecreaseIter = tempPetLevelDecreaseQueue.begin();
		createPetIter = tempCreatePetQueue.begin();

		petEquipIter = tempPetEquipQueue.begin();		
		takeCarePetBySchoolIter = tempTakeCarePetBySchoolQueue.begin();
		takeOutCaredPetIter = tempTakeOutCaredPetQueue.begin();
		equipIter = tempEquipQueue.begin();
		changeEquipToEquipIter = tempChangeEquipToEquipQueue.begin();
		createItemIter = tempCreateItemQueue.begin();
		cmdIter = tempCmdQueue.begin();
		rebirthIter = tempRebirthQueue.begin();
		petRenamingIter = tempPetRenamingQueue.begin();
		petUnsummonIter = tempPetUnsummonQueue.begin();
		swapEquipmentIter = tempSwapEquipmentQueue.begin();
		swapCashEquipmentIter = tempCashSwapEquipmentQueue.begin();
	}

	void CommandQueue::Clear()
	{
		AutoLock locker(cmdQueueCS);

		joinQueue.clear();
		useQueue.clear();
		createPetQueue.clear();
		petItemQueue.clear();
		petFeedQueue.clear();
		presentToPetQueue.clear();
		petLevelDecreaseQueue.clear();
		petEquipQueue.clear();		
		takeCarePetBySchoolQueue.clear();
		takeOutCaredPetQueue.clear();
		equipQueue.clear();
		changeEquipToEquipQueue.clear();
		createItemQueue.clear();
		cmdQueue.clear();
		rebirthQueue.clear();
		orderQueue.clear();
		petRenamingQueue.clear();
		petUnsummonQueue.clear();

		ClearTempQueue();
	}

	void CommandQueue::ClearTempQueue()
	{
		tempJoinQueue.clear();
		tempUseQueue.clear();
		tempCreatePetQueue.clear();
		tempPetItemQueue.clear();
		tempPetFeedQueue.clear();
		tempPresentToPetQueue.clear();
		tempPetLevelDecreaseQueue.clear();
		tempPetEquipQueue.clear();		
		tempTakeCarePetBySchoolQueue.clear();
		tempTakeOutCaredPetQueue.clear();
		tempEquipQueue.clear();
		tempChangeEquipToEquipQueue.clear();
		tempCreateItemQueue.clear();
		tempCmdQueue.clear();
		tempOrderQueue.clear();
		tempRebirthQueue.clear();
		tempPetRenamingQueue.clear();
		tempPetUnsummonQueue.clear();
		tempSwapEquipmentQueue.clear();
		tempCashSwapEquipmentQueue.clear();
	}

	void CommandQueue::Join(ILogic::PlayerInitialData* data, void* user)
	{
		JoinQueue queueItem;

		queueItem.user = user;
		queueItem.data = data;

		{
			AutoLock locker(cmdQueueCS);
			joinQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypeJoin);
		}
	}

	void CommandQueue::Use(uint32 userSerial, Database::Info::ItemInfo* info, int bag, int pos)
	{
		UseQueue queueItem;

		queueItem.userSerial = userSerial;
		queueItem.info= info;
		queueItem.bag = bag;
		queueItem.pos = pos;

		{
			AutoLock locker(cmdQueueCS);
			useQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypeUse);
		}
	}


	void CommandQueue::PetItemUse(uint32 userSerial, Database::Info::ItemInfo* info, int bag, int pos, GlobalSerial serial)
	{
		PetItemQueue queuePetItem;

		queuePetItem.Type = PetItemQueue::Type::Use;
		queuePetItem.UserSerial = userSerial;
		queuePetItem.Info= info;
		queuePetItem.Bag = bag;
		queuePetItem.Pos = pos;
		queuePetItem.ItemSerial = serial;

		{
			AutoLock locker(cmdQueueCS);
			petItemQueue.push_back(queuePetItem);
			orderQueue.push_back(QueueType::TypePetItemUse);
		}
	}

	void CommandQueue::DropPetItem(uint32 userSerial, XRated::GlobalSerial itemSerial)
	{
		PetItemQueue queuePetItem;

		queuePetItem.Type = PetItemQueue::Type::Drop;
		queuePetItem.UserSerial = userSerial;
		queuePetItem.Info= NULL;
		queuePetItem.Bag = 0;
		queuePetItem.Pos = 0;
		queuePetItem.ItemSerial = itemSerial;

		{
			AutoLock locker(cmdQueueCS);
			petItemQueue.push_back(queuePetItem);
			orderQueue.push_back(QueueType::TypePetItemUse);
		}
	}

	void CommandQueue::PetFeed( uint32 userSerial, const GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed )
	{
		PetFeedQueue queuePetFeed;

		queuePetFeed.UserSerial = userSerial;
		queuePetFeed.PetSerial  = petSerial;
		queuePetFeed.FoodAmount = foodAmount;
		queuePetFeed.FoodCount  = foodCount;
		queuePetFeed.OverFeed = overFeed;

		{
			AutoLock locker(cmdQueueCS);
			petFeedQueue.push_back(queuePetFeed);
			orderQueue.push_back(QueueType::TypePetFeed);
		}
	}

	void CommandQueue::GivePresentToPet( uint32 userSerial, const GlobalSerial& petSerial, const uint32& sellPrice, const uint32& count )
	{
		PresentToPetQueue queuePresentToPet;

		queuePresentToPet.UserSerial = userSerial;
		queuePresentToPet.PetSerial  = petSerial;
		queuePresentToPet.PresentSellPrice = sellPrice;
		queuePresentToPet.PresentItemCount = count;

		{
			AutoLock locker(cmdQueueCS);
			presentToPetQueue.push_back(queuePresentToPet);
			orderQueue.push_back(QueueType::TypePresentToPet);
		}
	}

	void CommandQueue::PetLevelDecrease(uint32 userSerial, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel)
	{
		PetLevelDecreaseQueue queuePetDecreaseLevel;

		queuePetDecreaseLevel.UserSerial = userSerial;
		queuePetDecreaseLevel.PetSerial  = petSerial;
		queuePetDecreaseLevel.DownLevel = downLevel;
		queuePetDecreaseLevel.OnlyMaxLevel = onlyMaxLevel;

		{
			AutoLock locker(cmdQueueCS);
			petLevelDecreaseQueue.push_back(queuePetDecreaseLevel);
			orderQueue.push_back(QueueType::TypeDecreasePetLevel);
		}
	}

	void CommandQueue::CreatePet( uint32 userSerial, const Pet& pet )
	{
		CreatePetQueue queueCreatePet;

		queueCreatePet.UserSerial = userSerial;
		queueCreatePet.Pet        = pet;

		{
			AutoLock locker(cmdQueueCS);
			createPetQueue.push_back(queueCreatePet);
			orderQueue.push_back(QueueType::TypeCreatePet);
		}
	}
	void CommandQueue::PetEquip(XRated::GlobalSerial petSerial, uint32 userSerial, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where)
	{		
		PetEquipQueue data;
		data.PetSerial = petSerial;		
		data.userSerial = userSerial;
		data.info = info;
		data.fields = fields;	
		data.where = where;

		{
			AutoLock locker(cmdQueueCS);
			petEquipQueue.push_back( data );
			orderQueue.push_back(QueueType::TypePetEquip);
		}
	}
	

	void CommandQueue::TakeCarePetBySchool( uint32 userSerial
										  , const uint32& petItemHash, const XRated::GlobalSerial& petSerial
										  , const Database::Enchant::EnchantBitfields& instance, const uint8& count
										  , const uint32& period, const float& expFactor )
	{
		TakeCarePetBySchoolQueue data;
		data.UserSerial  = userSerial;
		data.PetItemHash = petItemHash;
		data.PetSerial   = petSerial;
		data.PetItemInstance = instance;
		data.PetItemcount    = count;
		data.Period     = period;
		data.ExpFactor  = expFactor;

		{
			AutoLock locker(cmdQueueCS);
			takeCarePetBySchoolQueue.push_back( data );
			orderQueue.push_back(QueueType::TypeTakeCarePetBySchool);
		}
	}

	void CommandQueue::TakeOutCaredPet( uint32 userSerial, const XRated::GlobalSerial& petSerial )
	{
		TakeOutCaredPetQueue data;
		data.UserSerial = userSerial;
		data.PetSerial  = petSerial;

		{
			AutoLock locker(cmdQueueCS);
			takeOutCaredPetQueue.push_back( data );
			orderQueue.push_back(QueueType::TypeTakeOutCaredPet);
		}
	}

	void CommandQueue::Equip(uint32 userSerial, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where)
	{
		EquipQueue queueItem;
		queueItem.userSerial = userSerial;
		queueItem.info = info;
		queueItem.fields = fields;
		queueItem.where = where;

		{
			AutoLock locker(cmdQueueCS);
			equipQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypeEquip);
		}
	}

	void CommandQueue::ChangeEquipToEquip(uint32 name,Constants::Equipment from,Constants::Equipment to)
	{
		ChangeEquipToEquipQueue queueItem;
		queueItem.userSerial = name;
		queueItem.from = from;
		queueItem.to = to;
		{
			AutoLock locker(cmdQueueCS);
			changeEquipToEquipQueue.push_back(queueItem);
			orderQueue.push_back(TypeChangeEquipToEquip);
		}
	}

	void CommandQueue::CreateItem(uint32 name, const XRated::RewardItem& rewardItem, bool isPrivateItem)
	{
		CreateItemQueue queueItem;
		queueItem.userSerial = name;
		queueItem.rewardItem = rewardItem;
		queueItem.isPrivateItem = isPrivateItem;
		{
			AutoLock locker(cmdQueueCS);
			createItemQueue.push_back(queueItem);
			orderQueue.push_back(TypeCreateItem);
		}
	}

	void CommandQueue::Cmd(CmdQueue& queueItem)
	{
		AutoLock locker(cmdQueueCS);
		cmdQueue.push_back(queueItem);
		orderQueue.push_back(QueueType::TypeCmd);
	}

	void CommandQueue::Rebirth(XRated::Serial playerSerial,uint32 levelAfterRebirth)
	{
		RebirthQueue queueItem;
		queueItem.playerSerial = playerSerial;
		queueItem.levelAfterRebirth = levelAfterRebirth;

		{
			AutoLock locker(cmdQueueCS);
			rebirthQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypeRebirth);
		}
	}

	void CommandQueue::RebirthRollBack(XRated::Serial playerSerial)
	{
		RebirthQueue queueItem;
		queueItem.playerSerial = playerSerial;

		{
			AutoLock locker(cmdQueueCS);
			rebirthQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypeRebirthRollBack);
		}
	}
	
	void CommandQueue::PetRenaming(XRated::Serial playerSerial, XRated::GlobalSerial petSerial, const std::wstring& newName)
	{
		PetRenamingQueue queueItem;
		
		queueItem.playerSerial = playerSerial;
		queueItem.petSerial = petSerial;
		queueItem.newName = newName;

		{
			AutoLock locker(cmdQueueCS);
			petRenamingQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypePetRenaming);
		}
	}

	void CommandQueue::PetUnsummon(XRated::Serial playerSerial)
	{
		PetUnsummonQueue queueItem;

		queueItem.playerSerial = playerSerial;

		{
			AutoLock locker(cmdQueueCS);
			petUnsummonQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypePetUnsummon);
		}
	}
	
	void CommandQueue::SwapEquipment(uint32 userSerial, const std::vector< XRated::EquippedItem >& newEquipments)
	{
		SwapEquipmentQueue queueItem;

		queueItem.userSerial = userSerial;
		queueItem.equipments.clear();
		queueItem.equipments.assign( newEquipments.begin(), newEquipments.end() );

		{
			AutoLock locker(cmdQueueCS);
			swapEquipmentQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypeSwapEquipment);
		}
	}

	void CommandQueue::SwapCashEquipment(uint32 userSerial, const std::vector< XRated::EquippedItem >& newEquipments)
	{
		SwapCashEquipmentQueue queueItem;

		queueItem.userSerial = userSerial;
		queueItem.equipments.clear();
		queueItem.equipments.assign( newEquipments.begin(), newEquipments.end() );

		{
			AutoLock locker(cmdQueueCS);
			swapCashEquipmentQueue.push_back(queueItem);
			orderQueue.push_back(QueueType::TypeSwapCashEquipment);
		}
	}

	void CommandQueue::GetJoinItem(CommandQueue::JoinQueue& r)
	{
		r = *joinIter;
		++joinIter;
	}

	void CommandQueue::GetUseItem(CommandQueue::UseQueue& r)
	{
		r = *useIter;
		++useIter;
	}


	void CommandQueue::GetPetItem(PetItemQueue& r)
	{
		r = *petItemIter;
		++petItemIter;
	}

	void CommandQueue::GetPetFeed(PetFeedQueue& r)
	{
		r = *petFeedIter;
		++petFeedIter;
	}

	void CommandQueue::GetPresentToPetItem(CommandQueue::PresentToPetQueue& r)
	{
		r = *presentToPetIter;
		++presentToPetIter;
	}

	void CommandQueue::GetPetLevelDecrease(PetLevelDecreaseQueue& r)
	{
		r = *petLevelDecreaseIter;
		++petLevelDecreaseIter;
	}

	void CommandQueue::GetCreatePet(CreatePetQueue& r)
	{
		r = *createPetIter;
		++createPetIter;
	}
	void CommandQueue::GetPetEquip(PetEquipQueue& r)
	{
		r = *petEquipIter;
		++petEquipIter;
	}

	void CommandQueue::GetTakeCarePetBySchool(TakeCarePetBySchoolQueue& r)
	{
		r = *takeCarePetBySchoolIter;
		++takeCarePetBySchoolIter;
	}

	void CommandQueue::GetTakeOutCaredPet(TakeOutCaredPetQueue& r)
	{
		r = *takeOutCaredPetIter;
		++takeOutCaredPetIter;
	}

	void CommandQueue::GetEquipItem(CommandQueue::EquipQueue& r)
	{
		r = *equipIter;
		++equipIter;
	}

	void CommandQueue::GetCmdItem(CommandQueue::CmdQueue& r)
	{
		r = *cmdIter;
		++cmdIter;
	}

	void CommandQueue::GetChangeEquipToEquipItem(ChangeEquipToEquipQueue& r)
	{
		r = *changeEquipToEquipIter;
		++changeEquipToEquipIter;
	}
	void CommandQueue::GetCreateItem(CommandQueue::CreateItemQueue& r)
	{
		r = *createItemIter;
		++createItemIter;
	}

	void CommandQueue::GetRebirth(RebirthQueue& r)
	{
		r = *rebirthIter;
		++rebirthIter;
	}

	void CommandQueue::GetPetRenaming(PetRenamingQueue& r)
	{
		r = *petRenamingIter;
		++petRenamingIter;
	}

	void CommandQueue::GetPetUnsummon(PetUnsummonQueue& r)
	{
		r = *petUnsummonIter;
		++petUnsummonIter;
	}
	
	void CommandQueue::GetSwapEquipment(SwapEquipmentQueue& r)
	{
		r = *swapEquipmentIter;
		++swapEquipmentIter;
	}

	void CommandQueue::GetCashSwapEquipment( SwapCashEquipmentQueue& r )
	{
		r = *swapCashEquipmentIter;
		++swapCashEquipmentIter;
	}


}	}	}

