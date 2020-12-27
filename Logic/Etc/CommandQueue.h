#include "../Logic.h"
#include "../Object/Player.h"
#include <vector>

namespace Lunia { namespace XRated {	namespace Logic {

	class CommandQueue {
	public :

		enum QueueType { TypeJoin, TypeUse, TypePetItemUse, TypePetFeed, TypeEquip, TypeCmd, TypePresentToPet, TypeChangeEquipToEquip, TypeCreateItem
			           , TypeDropPetItem, TypeDecreasePetLevel, TypeCreatePet, TypePetEquip, TypeTakeCarePetBySchool, TypeTakeOutCaredPet, TypePetRenaming, TypeRebirth, TypeRebirthRollBack, TypePetUnsummon
					   , TypeSwapEquipment, TypeSwapCashEquipment };

		struct JoinQueue {
			void* user;
			ILogic::PlayerInitialData* data;
		};

		struct UseQueue {
			uint32 userSerial;
			const Database::Info::ItemInfo* info;
			int bag;
			int pos;
		};


		struct PetItemQueue {
			enum Type {
				Use,
				Drop,
			};
			Type Type;
			uint32 UserSerial;
			const Database::Info::ItemInfo* Info;
			int Bag;
			int Pos;
			GlobalSerial ItemSerial;
		};

		struct PetFeedQueue {
			uint32 UserSerial;
			GlobalSerial PetSerial;
			uint32 FoodAmount;
			uint32 FoodCount;
			bool OverFeed;
		};

		struct PresentToPetQueue {
			uint32       UserSerial;
			GlobalSerial PetSerial;
			uint32       PresentSellPrice;
			uint32       PresentItemCount;
		};

		struct PetLevelDecreaseQueue
		{
			uint32 UserSerial;
			GlobalSerial PetSerial;
			uint32 DownLevel;
			uint32 OnlyMaxLevel;
		};

		struct CreatePetQueue
		{
			uint32 UserSerial;
			Pet    Pet;
		};

		struct PetEquipQueue {
			uint32 userSerial;
			XRated::GlobalSerial	PetSerial;

			const Database::Info::ItemInfo* info;
			Database::Enchant::EnchantBitfields fields;
			Constants::Equipment where;
		};		

		struct TakeCarePetBySchoolQueue
		{
			uint32		 UserSerial;
			uint32       PetItemHash;
			GlobalSerial PetSerial;
			uint64       PetItemInstance;
		    uint8        PetItemcount;
			uint32       Period;
			float        ExpFactor;
		};

		struct TakeOutCaredPetQueue
		{
			uint32       UserSerial;
			GlobalSerial PetSerial;
		};

		struct EquipQueue {
			uint32 userSerial;

			const Database::Info::ItemInfo* info;
			Database::Enchant::EnchantBitfields fields;
			Constants::Equipment where;
		};

		struct ChangeEquipToEquipQueue {
			uint32 userSerial;
			Constants::Equipment from;
			Constants::Equipment to;
		};

		struct CreateItemQueue {
			uint32 userSerial;
			XRated::RewardItem rewardItem;
			bool isPrivateItem;
		};

		struct CmdQueue {
			uint32 userSerial;

			Constants::Command operation;
			uint32 param;
			uint32 param2;				//< arcrus. 즉시부활 용. 밑에 저거 리팩토링 하게 되면 같이 낑겨서 해보자. 밑에거를 쓸수도 있지만, 그냥 이거 쓴다.
			uint32 subParam;			//< 이것은 exp만을 위해서 만들어진 param이다. 2007.11.14 kwind -> 추후 queue system을 리팩토링 한다.
		};

		struct RebirthQueue
		{
			uint32 playerSerial;
			uint32 levelAfterRebirth;
		};

		struct PetRenamingQueue
		{
			uint32 playerSerial;
			uint64 petSerial;
			std::wstring newName;
		};

		struct PetUnsummonQueue
		{
			uint32 playerSerial;
		};
		
		struct SwapEquipmentQueue 
		{
			uint32 userSerial;
			
			std::vector< EquippedItem > equipments;
		};

		struct SwapCashEquipmentQueue 
		{
			uint32 userSerial;
			
			std::vector< EquippedItem > equipments;
		};


	protected :
		std::vector<JoinQueue>	joinQueue;
		std::vector<UseQueue>	useQueue;
		std::vector<PetItemQueue> petItemQueue;
		std::vector<PetFeedQueue> petFeedQueue;
		std::vector<PetLevelDecreaseQueue> petLevelDecreaseQueue;
		std::vector<PresentToPetQueue> presentToPetQueue;		
		std::vector<CreatePetQueue> createPetQueue;
		std::vector<PetEquipQueue> petEquipQueue;		
		std::vector<TakeCarePetBySchoolQueue> takeCarePetBySchoolQueue;
		std::vector<TakeOutCaredPetQueue>     takeOutCaredPetQueue;
		std::vector<EquipQueue>	equipQueue;
		std::vector<ChangeEquipToEquipQueue>	changeEquipToEquipQueue;
		std::vector<CreateItemQueue>	createItemQueue;
		std::vector<CmdQueue>	cmdQueue;
		std::vector<RebirthQueue>	rebirthQueue;
		std::vector<PetRenamingQueue> petRenamingQueue;
		std::vector<PetUnsummonQueue> petUnsummonQueue;
		std::vector<QueueType>	orderQueue;
		std::vector<SwapEquipmentQueue> swapEquipmentQueue;
		std::vector<SwapCashEquipmentQueue> swapCashEquipmentQueue;

		std::vector<JoinQueue>	tempJoinQueue;
		std::vector<UseQueue>	tempUseQueue;
		std::vector<PetItemQueue> tempPetItemQueue;
		std::vector<PetFeedQueue> tempPetFeedQueue;
		std::vector<PetLevelDecreaseQueue> tempPetLevelDecreaseQueue;
		std::vector<PresentToPetQueue> tempPresentToPetQueue;
		std::vector<CreatePetQueue> tempCreatePetQueue;
		std::vector<PetEquipQueue> tempPetEquipQueue;		
		std::vector<TakeCarePetBySchoolQueue> tempTakeCarePetBySchoolQueue;
		std::vector<TakeOutCaredPetQueue>     tempTakeOutCaredPetQueue;
		std::vector<EquipQueue>	tempEquipQueue;
		std::vector<ChangeEquipToEquipQueue>	tempChangeEquipToEquipQueue;
		std::vector<CreateItemQueue>	tempCreateItemQueue;
		std::vector<CmdQueue>	tempCmdQueue;
		std::vector<RebirthQueue>	tempRebirthQueue;
		std::vector<PetRenamingQueue> tempPetRenamingQueue;
		std::vector<PetUnsummonQueue> tempPetUnsummonQueue;
		std::vector<QueueType>	tempOrderQueue;
		std::vector<SwapEquipmentQueue> tempSwapEquipmentQueue;
		std::vector<SwapCashEquipmentQueue> tempCashSwapEquipmentQueue;

		std::vector<JoinQueue>::iterator	joinIter;
		std::vector<UseQueue>::iterator		useIter;
		std::vector<PetItemQueue>::iterator petItemIter;
		std::vector<PetFeedQueue>::iterator petFeedIter;
		std::vector<PetLevelDecreaseQueue>::iterator petLevelDecreaseIter;
		std::vector<PresentToPetQueue>::iterator presentToPetIter;
		std::vector<CreatePetQueue>::iterator createPetIter;
		std::vector<PetEquipQueue>::iterator petEquipIter;		
		std::vector<TakeCarePetBySchoolQueue>::iterator takeCarePetBySchoolIter;
		std::vector<TakeOutCaredPetQueue>::iterator     takeOutCaredPetIter;

		std::vector<EquipQueue>::iterator	equipIter;
		std::vector<ChangeEquipToEquipQueue>::iterator changeEquipToEquipIter;
		std::vector<CreateItemQueue>::iterator	createItemIter;
		std::vector<CmdQueue>::iterator		cmdIter;
		std::vector<RebirthQueue>::iterator		rebirthIter;
		std::vector<PetRenamingQueue>::iterator petRenamingIter;
		std::vector<PetUnsummonQueue>::iterator petUnsummonIter;

		std::vector<SwapEquipmentQueue>::iterator swapEquipmentIter;
		std::vector<SwapCashEquipmentQueue>::iterator swapCashEquipmentIter;

		std::mutex cmdQueueCS;

	public :
		CommandQueue();

		void Clear();
		void ClearTempQueue();
		void SwapQueue();

		void Join(ILogic::PlayerInitialData* data, void* user);
		void Use(uint32 name, Database::Info::ItemInfo* info, int bag, int pos);

		void PetItemUse(uint32 userSerial, Database::Info::ItemInfo* info, int bag, int pos, GlobalSerial serial);
		void DropPetItem(uint32 userSerial, XRated::GlobalSerial itemSerial);
		void PetFeed( uint32 userSerial, const GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed = false );
		void GivePresentToPet( uint32 userSerial, const GlobalSerial& petSerial, const uint32& sellPrice, const uint32& count );
		void PetLevelDecrease(uint32 userSerial, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel);
		void CreatePet( uint32 userSerial, const Pet& pet );

		void PetEquip(XRated::GlobalSerial petSerial, uint32 userSerial, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where);		

		void TakeCarePetBySchool( uint32 userSerial
								, const uint32& petItemHash, const XRated::GlobalSerial& petSerial
								, const Database::Enchant::EnchantBitfields& instance, const uint8& count
								, const uint32& period, const float& expFactor );
		void TakeOutCaredPet( uint32 userSerial, const XRated::GlobalSerial& petSerial );

		void Equip(uint32 name, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where);

		void ChangeEquipToEquip(uint32 name,Constants::Equipment from,Constants::Equipment to);
		void CreateItem(uint32 name, const XRated::RewardItem& rewardItem, bool isPrivateItem);
		void Cmd(CmdQueue& queueItem);
		void Rebirth(XRated::Serial playerSerial,uint32 levelAfterRebirth);
		void RebirthRollBack(XRated::Serial playerSerial);
		void PetRenaming(XRated::Serial playerSerial, XRated::GlobalSerial petSerial, const std::wstring& newName);
		void PetUnsummon(XRated::Serial playerSerial);

		void SwapEquipment(uint32 userSerial, const std::vector< XRated::EquippedItem >& newEquipments);
		void SwapCashEquipment(uint32 userSerial, const std::vector< XRated::EquippedItem >& newEquipments);

		std::vector<QueueType>& GetOrderQueue() { return tempOrderQueue; }
		void GetJoinItem(JoinQueue&);
		void GetUseItem(UseQueue&);


		void GetPetItem(PetItemQueue&);
		void GetPetFeed(PetFeedQueue&);
		void GetPresentToPetItem(PresentToPetQueue&);
		void GetPetLevelDecrease(PetLevelDecreaseQueue&);
		void GetCreatePet(CreatePetQueue& r);

		void GetPetEquip(PetEquipQueue& );		
		void GetTakeCarePetBySchool(TakeCarePetBySchoolQueue& );
		void GetTakeOutCaredPet(TakeOutCaredPetQueue& );


		void GetEquipItem(EquipQueue&);
		void GetCmdItem(CmdQueue&);

		void GetChangeEquipToEquipItem(ChangeEquipToEquipQueue&);
		void GetCreateItem(CreateItemQueue&);

		void GetRebirth(RebirthQueue&);
		
		void GetPetRenaming(PetRenamingQueue&);

		void GetPetUnsummon(PetUnsummonQueue&);

		void GetSwapEquipment(SwapEquipmentQueue& r);
		void GetCashSwapEquipment(SwapCashEquipmentQueue& r);

		int GetCmdQueueCnt() const { return (int)tempOrderQueue.size(); }
	};


}	}	}

