#pragma once
#include "Bag.h"
#include <StageServer/Common.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {

			class Inventory
			{
			public:
				Inventory(uint32 maxBag, uint32 slotCount, bool isBank);
				~Inventory();

				void Clear();

				const Common::ItemEx* GetItem(uint32 bag, uint32 pos) const; ///< @return NULL if the position is empty or invalid
				const Common::ItemEx* GetItem(const ItemPosition& pos) const;
				std::vector<const Common::ItemEx*> GetItems(const Common::ItemEx::IFindCondition& condition, bool includeDisabled = false) const;
				std::vector<const Common::ItemEx*> GetItemsInNormalBag(const Common::ItemEx::IFindCondition& condition, bool includeDisabled = false) const;

				uint32 GetCount(const Common::ItemEx::ICountCondition& condition) const;
				uint32 GetItemCount(uint32 itemHash) const; // on whole of inventory
				uint32 GetEmptySlotCount() const;
				uint32 GetBlankSlotCountOfNormalBag() const;
				uint32 GetEmptySlotCountByType(const Database::Info::ItemInfo* item) const;

				XRated::ItemPosition FindItemPosition(const Common::ItemEx::IFindCondition& condition, bool checkDisabledBag = false) const;
				XRated::ItemPosition FindEmptySlotByType(const Common::ItemEx::IFindCondition& condition, const Database::Info::ItemInfo* item, bool checkDisabledBag = false) const;
				std::vector<XRated::ItemPosition> FindItemPositions(const Common::ItemEx::IFindCondition& condition) const;
				std::vector<XRated::ItemPosition> FindEmptySlotsByType(const Common::ItemEx::IFindCondition& condition, const Database::Info::ItemInfo* item) const;

				XRated::ItemPosition FindEmptySlot(const Database::Info::ItemInfo* item) const;
				XRated::ItemPosition FindStackableSlot(const Database::Info::ItemInfo* item, InstanceEx instance, uint16 count) const;
				std::vector<XRated::ItemPosition> FindStackableSlots(const Database::Info::ItemInfo* item, InstanceEx instance, uint16 count, bool checkEmptySlot = true) const;
				XRated::ItemPosition FindItem(uint32 itemHash) const;
				XRated::ItemPosition FindPetItem(XRated::GlobalSerial itemSerial);

				uint32 CountRemainStackableItem(uint32 itemHash, InstanceEx instance, uint32 count) const;
				uint32 CountRemainAfterAddItem(uint32 itemHash, InstanceEx instance, uint32 count, uint32 maxCnt) const;

				bool Exist(uint32 bag, uint32 pos) const;
				bool SetItem(uint32 bag, uint32 pos, const Common::ItemEx& item);
				bool AddItem(uint32 bag, uint32 pos, uint32 count);
				bool DropItem(uint32 bag, uint32 pos, uint32 count, Common::ItemEx* droppedItem = NULL);
				void DeleteItem(uint32 bag, uint32 pos);
				uint32 DeleteItems(uint32 itemHash, uint32 count); ///< @return deleted item count
				bool MoveItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos);
				bool StackItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos, uint32 count);
				void AllItems(std::vector< Common::ItemSlotEx >& allItems);
				void AllItems(std::vector< XRated::ItemSlot >& allItems);
				void GetAllItemSlots(std::vector< const std::vector<Common::ItemEx>* >& slots);
				XRated::ItemPosition GetUnsortedItemPosition(XRated::ItemPosition start);
				//XRated::ItemPosition GetNextEmptySlot(XRated::ItemPosition start);

				void Init();
				void RenewBag(int bagNuimber, const DateTime& expireDate = DateTime::Infinite);
				void GetBags(std::vector<uint8>& bags) const;
				void ResetEnable(int bagNumber);
				DateTime GetBagExpireDate(int bagNumber) const;
				bool IsEnable(int bagNumber) const;

				uint16 GetNumberOfBags() const;
				bool SetItemInstance(uint32 bag, uint32 pos, InstanceEx instance);
				const Common::ItemEx* ItemInfoChange(uint8 bag, uint8 position, const Database::Info::ItemInfo* info);
				bool IsEnableItem(uint8 bagNumber, const Database::Info::ItemInfo* item) const;

			private:
				typedef std::map< int , Bag > Bags;
				Bags bags;
				const uint32 MAXBAG;
				const uint32 SLOTCOUNT;
				const bool	ISBANK;
			};


		}
	}
}