#pragma once

#include "ItemRequirementQueue.h"
#include "Inventory.h"
#include "Equip.h"
#include "PetInventory.h"
#include <Logic/Logic.h>
#include <StageServer/Protocol/ToServer.h>

#ifndef BAN_DUPLICATED_ITEM_SERIAL
#define BAN_DUPLICATED_ITEM_SERIAL
#endif

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			
			class User;
			class IUserRoom;
			
			class ItemException : public Exception
			{
			public:
				ItemException(const wchar_t* message) : Exception(message) {}
			};

			class Items
			{
			public:
				struct ChangedItems {
					std::vector< Common::ItemSlotEx >	Insert;
					std::vector< Common::ItemSlotEx >	Update;
					std::vector< Common::ItemSlotEx >	Delete;

					void Clear();
				};
			public:
				struct Constants
				{
					static const uint16 InvalidPosition = 0xffff;

					enum
					{
						//�Ʒ��� ������ XRated::Constants�� �ִ� ������ ����. by kwind. 2009-01-19
						//NumberOfInventoryBags=5, MaxOfSlotsInInventoryBag=20,
						//NumberOfBankBags=5, NumberOfSlotsInBank=20, NumberOfSlotsInEquipment= 27,
						EquipmentBagNumber = 0,
					};
				};

				struct Validation {
					struct ItemPosition
					{
						uint32 Bag;
						uint32 Position;

						ItemPosition(uint32 b, uint32 p) : Bag(b), Position(p) {}
						ItemPosition(const XRated::ItemPosition& item) : Bag(item.Bag), Position(item.Position) {}
					private:
						ItemPosition() : Bag(0xFFFFFFFF), Position(0xFFFFFFFF) {}
					};

					static bool IsValidPacketParameter(const Validation::ItemPosition& pos, bool bank = false)
					{
						if (bank)
						{
							if (pos.Bag < XRated::Constants::MaxOfBankBags && pos.Position < XRated::Constants::MaxOfSlotsInBankBag)
								return true;
						}
						else
						{
							if (pos.Bag == Items::Constants::EquipmentBagNumber && pos.Position < XRated::Constants::Equipment::Cnt)
								return true;
							if (pos.Position < XRated::Constants::MaxOfSlotsInInventoryBag)
								return true;
						}
						return false;
					};
				};

				class IEventListener
				{
				public:
					//virtual bool IsAchievementConnected() = 0;
					virtual const std::wstring& GetName() const = 0;
					virtual void ItemsSend(Protocol::IPacketSerializable& packet) = 0;
					//virtual void ItemsSendAchievement(Protocol::IPacketSerializable& packet) = 0;
					//virtual void ItemAcquiredAchievement(Net::Protocol::Achievement::ServerProtocol::ItemAcquired packet) = 0;
					virtual void ItemsSendToAll(Protocol::IPacketSerializable& packet) = 0;
					virtual void ItemsDirectSend(Protocol::IPacketSerializable& packet) = 0;
					virtual bool ItemsUse(const uint32& itemHash, const uint32& bag, const uint32& pos) = 0;

					virtual bool PetItemUse(const uint32& itemHash, const uint32& bag, const uint32& pos, GlobalSerial itemSerial) = 0;
					virtual void PetInventoryDirectSend(Protocol::IPacketSerializable& packet) = 0;
					virtual bool PetItemsEquip(XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, const Database::Enchant::EnchantBitfields& instance, const uint32& equipPos) = 0;
					virtual bool PetItemPositionUpdate(XRated::GlobalSerial petSerial, const uint8& bag, const uint8& position) = 0;

					virtual void RemoveDuplicatedItem(const uint32& hash, const uint64 serial, const uint8& bag, const uint8& pos, const uint32& stackcount, const InstanceEx& instance, int isBankItem, bool insertAgainAfterDelete) = 0;///< arcrus. forced character item info. update.

					virtual bool ItemsEquip(const Database::Info::ItemInfo* info, const Database::Enchant::EnchantBitfields& instance, const uint32& equipPos) = 0;
					virtual bool ItemsEquipToEquip(const uint32& from, const uint32& to) = 0;
					virtual bool EquipmentSwap(const std::vector< EquippedItem >& newEquipments) = 0;
					virtual bool CashEquipmentSwap(const std::vector< EquippedItem >& newEquipments) = 0;
					virtual Database::Info::SkillInfoList::SkillInfo* GetSkillInfo(const uint32& skillGroupName) const = 0;
					virtual const XRated::Constants::ClassType& GetClassType() const = 0;
					virtual void CriticalError(const char* logMessage, const bool& block, const uint32& blockDurationInSec) = 0;
					virtual void SendSystemChat(const std::wstring& message) = 0;
					virtual const Database::Info::StageInfo* GetStageInfo() const = 0;
					virtual void SetEquipmentIndex(const uint8& newIndex) = 0;
					virtual void SetCashEquipmentIndex(const uint8& newIndex) = 0;
					virtual void SendItemMail(const uint32& itemHash, uint16 stackCount, const InstanceEx& instance) = 0;
					// BMS 2334 by Yong-Eun, Jung
					//virtual const Common::ItemEx* GetTryEquipItemInCashItemStorage() = 0;
				};

			public:
				Items(IEventListener& listener);
				~Items();

				void SetPlayer(Logic::Player* player);
				void DeletePlayer();
				void Clear();

			public:
				int SetItemList(const json& Bags, const json& Items); ///< @return number of expired item
				bool IsEnable(bool isBank, int bagNumber) const; ///< @return false if the bag is disabled
				DateTime RenewBag(bool isBank, int bagNumber, int days); ///< @return next expiration date
				DateTime GetExpireDate(bool isBank, int bagNumber) const; ///< @return current expireation date
				uint16 GetNumberOfBags(bool isBank) const; ///< @return number of bags including given(basic) bag (means returns more than 0 always)
				uint32 GetBagsSlots(bool isBank); ///< @return number of bags including given(basic) bag (means returns more than 0 always)
				void ResetExpiredBag();

				bool SortInventory();

				const Common::ItemEx* GetItem(uint32 bag, uint32 pos, bool isBank = false) const;
				const Common::ItemEx* GetItemInPetInven(const XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType type, uint8 pos) const;
				//uint8 GetStackedCount(uint32 bag, uint32 pos) const;
				void SetItem(uint32 bag, uint32 pos, const Common::ItemEx& item, bool sendPacket = false);
				void GetEquip(std::vector<Common::ItemEx>& equipments) const;
				// from User (ToStageServer packet)
				bool SwapEquipment(uint8 set);
				bool SwapCashEquipment(uint8 set);
				bool ChangeEquipSet(uint8 set);
				bool ChangeCashEquipSet(uint8 set);
				bool MoveItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos);
				bool StackItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos, uint32 count);
				bool UseItem(uint32 bag, uint32 pos);

				bool UsePetItem(uint32 bag, uint32 pos);




				/** DropItems
					@bag bag position
					@pos bag in a position
					@droppedItem(OUTPUT) NULL is not use,this parameter used only inventory(not use equipment slot), stackCount = left over count(current count)
					@return dropped:true, not excuted: false
				*/
				bool DropItems(uint32 bag, uint32 pos, uint32 count, bool sendPacket, Common::ItemEx* droppedItem = NULL);  ///< @return false by invalid count 
				bool DropItemsInPetInventory(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType posType, uint8 pos, uint32 count, bool sendPacket, Common::ItemEx* droppedItem = NULL);  ///< @return false by invalid count 
				bool MoveItemInventoryToPet(const XRated::GlobalSerial& petSerial, uint8 inventoryBag, uint8 inventoryPosotion, XRated::PetItemSlot::PositionType positionType, uint8 petInventoryPosition);
				bool MoveItemInPetInven(const XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType sourcePositionType, uint8 sourcePosition, XRated::PetItemSlot::PositionType targetPositionType, uint8 targetPosition);
				bool UseItemInPetInven(const XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType positionType, uint8 position);
				bool TakeOutAllPetItems(const XRated::GlobalSerial petSerial);

				const Common::ItemEx* GetPetItem(XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType positionType, uint8 position);

				uint16 PetEquipPutOned(PetMoveEquipRequirement& result); ///< @return equipped position, and returns Constants::InvalidPosition by invalid equip command
				uint16 PetEquipTakeOffed(PetMoveEquipRequirement& result); ///< @return unequipped position, and returns Constants::InvalidPosition by invalid unequip command

				//void DeleteItems(uint32 bag, uint32 pos, uint32 count, bool sendPacket); ///< force drop though count is bigger than exists
				bool Buy(Protocol::ToServer::Buy& buy, bool isPcRoom, float discountRate = 1.0f);
				bool BuyBack(Protocol::ToServer::BuyBack& buy, bool isPcRoom, float discountRate = 1.0f); // 3.1 BY ULTIMATE
				bool BuyBarterItem(Protocol::ToServer::BuyBarterItem& buy, bool isPcRoom); // 3.1 BY ULTIMATE
				bool Sell(Protocol::ToServer::Sell& sell, bool isPcRoom);
				bool DropBankItems(uint32 bag, uint32 pos);
				bool Identify(uint32 bag, uint32 pos, XRated::Item* acquiredItem/*Out*/);
				bool MissionClearItemIdentify(const Database::Info::ItemInfo* itemInfo, XRated::RewardItem& rewardItem);
				bool GetIdentifiedItem(uint32 itemHash, XRated::ItemBasicInfo& resultItem);

				// BMS 2334 by Yong-Eun, Jung
				//bool EquipItem(uint32 pos, const Common::ItemEx& item);
				static bool IsAbleToUse(const Logic::Player* player, const Database::Info::ItemInfo* info, const Database::Info::StageInfo* stageInfo);
				// LogicListener ���� ����
				bool ItemAcquired(const uint32 id, Serial gameObjectSerial, uint32 count, InstanceEx instance = 0, bool autoStack = true, XRated::Constants::AddItemType addItemType = XRated::Constants::AddItemType::NoneAddItemType, uint64 itemSerial = 0, bool addSeparate = false, Item* identifiedAcquireItem = NULL);
				bool ItemUsed(uint32 returnValue/*Logic::Player::ItemUseResult*/, const Database::Info::ItemInfo* info, uint32 bag, uint32 pos);
				bool IsItemAddable(const Database::Info::ItemInfo* info, InstanceEx instance, uint32 count, bool autoStack = true) const;
				uint32 CountRemainStackableItem(const Database::Info::ItemInfo* info, InstanceEx instance, uint32 count) const;
				uint32 CountRemainAfterAddItem(const Database::Info::ItemInfo* info, InstanceEx instance, uint32 count, uint32 maxCnt) const; // return ���� �κ��� ����ְ� ���� �� ���� ���� �������� ��

				size_t ReagentItemUsed(uint32 skillGroupName);
				uint16 ItemEquiped(bool succeed); ///< @return equipped position, and returns Constants::InvalidPosition by invalid equip command
				void ItemEquipToEquiped(bool succeed); ///< @return equipped position, and returns Constants::InvalidPosition by invalid equip command
				void EquipmentSwapped(bool succeed);
				void CashEquipmentSwapped(bool succeed);
				uint16 EquipDroped(IItemResult& result);
				uint16 EquipPutOned(IItemResult& result); ///< @return equipped position, and returns Constants::InvalidPosition by invalid equip command
				uint16 EquipTakeOffed(IItemResult& result); ///< @return unequipped position, and returns Constants::InvalidPosition by invalid unequip command
				void EquipToEquiped(IItemResult& result);
				void SwapNewEquipment(IItemResult& result);
				uint16 PetInventoryToEquiped(IItemResult& result);
				// BMS 2334 by Yong-Eun, Jung
				uint16 DirectlyEquiped(IItemResult& result);
				bool Remove(uint32 itemHash, uint32 count);
				uint16 Remove(const XRated::ItemPosition& pos, uint32 itemHash, uint16 count, bool sendPacket = true, bool writeLog = true);
				//bool RemoveItemState(const Logic::Player* player, const Database::Info::ItemInfo* info);
				bool RemoveItemsByAttribute(uint16 attribute, bool checkDisabledBag = false, bool writeWebLog = false);
				bool RemoveStageItems(); ///< @return true if stage item exist

				uint32 GetEmptySlotCount() const;
				uint32 GetBlankSlotCountOfNormalBag() const;
				int GetItemCount(uint32 itemHash) const;
				int GetItemCountInPetInventory(uint32 itemHash, bool onlyAppear) const;
				int GetItemCountInPetInventory(const Database::Info::ItemInfo::IFindCondition& condition, bool onlyAppear) const;
				int GetRequiredSlotCount(const std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> >& toRemove, const std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> >& toAdd, uint32 availablecount) const;
				uint32 GetItemCount(const Database::Info::ItemInfo::IFindCondition& condition, bool includeEquipped = false) const;
				ItemPosition FindItem(const Common::ItemEx::IFindCondition& condition, bool includeEquipped = false) const; ///< @return ItemPosition::Invalid if the item could not be found.
				ItemPosition FindBankItem(const Common::ItemEx::IFindCondition& condition) const; ///< @return ItemPosition::Invalid if the item could not be found.
				Common::PetItemSlotEx FindItemInPetInventory(uint32 itemHash, bool onlyAppear) const;



				const Common::ItemEx* ItemInfoChangeToInventory(uint8 bag, uint8 position, const Database::Info::ItemInfo* info);
				ItemPosition FindPetItem(XRated::GlobalSerial itemSerial);
				bool ChangeItemInstance(uint32 bag, uint32 pos, InstanceEx instance);

				int SetPetsInventoryItem(const std::string& items, std::vector< XRated::Pet >& pets);
				void AddInventory(const XRated::Pet& pet);
				bool IsItemInPetInventory(const GlobalSerial petSerial);
				bool CheckInventoryFull(void);

				XRated::ItemPosition AddItem(const Database::Info::ItemInfo* info, uint32 count = 1, InstanceEx instance = 0, bool autoStack = true); ///< @return position to be added. returns ItemPosition::Invalid by failure.
				XRated::ItemPosition AddItem(const Common::ItemEx& item);
				std::vector<XRated::ItemPosition> AddItems(const Database::Info::ItemInfo* info, uint32 count, InstanceEx instance, bool autoStack = true);
				bool AddItem(XRated::ItemPosition pos, const Database::Info::ItemInfo* info, uint32 count = 1, InstanceEx instance = 0);
				void DropItem(XRated::ItemPosition position);
				std::vector<const Database::Info::ItemInfo*> GetPassiveItems() const;

				// arcrus. Fill inventory with designated item.
				uint32 FillItem(uint32 hash, uint32 amount);

				// etc
				bool IsExpiredItem();
				void AllItems(std::vector< Common::ItemSlotEx >& allItems);
				void AllChangedItemsAndOriginBackup(ChangedItems& changedItems, PetsInventory::ChangedItems& petsChangedItems, std::vector< Common::PetItemSlotEx >& petsAllItems
					, std::vector< Common::ItemSlotEx >& equipSlots, std::vector< Common::ItemSlotEx >& inventorySlots, std::vector< Common::ItemSlotEx >& bankSlots);
				void RebirthChangedItemsAndOriginBackup(ChangedItems& changedItems, std::vector< Common::ItemSlotEx >& inventorySlots);
				void RebirthRollBack();
				void RebirthBackUp();
				//void GetAllItems(std::vector< Common::ItemSlotEx >& equip, std::vector< Common::ItemSlotEx >& inventory, std::vector< Common::ItemSlotEx >& bank );
				void BankItemList(std::vector< XRated::ItemSlot >& listBankItem);
				bool RequirePutOnEquip(uint32 invenBag, uint32 invenPos, uint32 equipPos);
				bool RequireTakeOffEquip(uint32 equipPos, uint32 invenBag, uint32 invenPos);
				bool RequireDropEquip(uint32 equipPos);
				bool RequireEquipToEquip(uint32 fromEquipPos, uint32 toEquipPos);
				bool IsItemRequirement();// <- bool IsEquipRequirement();
				bool EquipDirectly(uint32 itemHash, InstanceEx instance, uint64 serial, uint32 equipPos);

				SlotPos GetFirstBlankSlot();
				//static bool IsExpired(const Database::Info::ItemInfo* info, int64 instance);

				std::mutex& GetSync() { return cs; }

			private:
				void SenditemRequirementError();
				void Send(Protocol::IPacketSerializable& packet);
				void SendToAll(Protocol::IPacketSerializable& packet);
				void Log(const char* operation, const Common::ItemEx& item);

				static bool IsImportantToLog(const Database::Info::ItemInfo* info);

				void GetProbabilityTable(XRated::UnidentifiedItemInfo* uInfo, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions);
				void GetClassSpecializedItemTable(const XRated::UnidentifiedItemInfo::ProbabilityTableList& originTable, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions);
				void GetMajorSpecializedItemTable(const XRated::UnidentifiedItemInfo::ProbabilityTableList& originTable, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions);
				void GetTotalProbabilityTable(const XRated::UnidentifiedItemInfo::ProbabilityTableList& originTable, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions);

				void ProcessInstantItem(const Database::Info::ItemInfo* itemInfo, uint16 stackCount = 1);

			private:
				long		itemTmpSerial;

				Inventory	inventory;
				Inventory	bank;
				Equip		equip;
				PetsInventory	petsInventory;
				mutable std::mutex cs; // items can be changed by io, net-bg and room updater thread.

				// User Info
				Logic::Player* player;   // Logic
				IEventListener& event;

				ItemRequirementQue itemRequirement;



			private:
				struct AllItemSlots {
					std::vector< Common::ItemSlotEx >	Equip;
					std::vector< Common::ItemSlotEx >	Inventory;
					std::vector< Common::ItemSlotEx >	Bank;

					void SetBankItem(uint32 bag, uint32 pos, const Common::ItemEx& item);
					void SetInventoryItem(uint32 bag, uint32 pos, const Common::ItemEx& item);
					void SetEquip(uint8 bag, uint32 pos, const Common::ItemEx& item);

					inline void Clear();
					AllItemSlots();
				};

				AllItemSlots	originSlots;

				void ChangedEquipSlotFilter(std::vector< Common::ItemSlotEx >& nowEquip, ChangedItems& in);
				void ChangedInventorySlotFilter(std::vector< Common::ItemSlotEx >& nowInventory, ChangedItems& in);
				void ChangedBankSlotFilter(std::vector< Common::ItemSlotEx >& nowBank, ChangedItems& in);

				std::vector< Common::ItemSlotEx >	rebirthRollBackInventory;
			};
		}
	}
}