#pragma once
#include "Slot.h"
#include <StageServer/Common.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {

			const Database::Info::ItemInfo* GetItemInfo(uint32 itemHash);
			const Database::Info::ItemInfo* GetItemInfo(const wchar_t* itemID);

			class Bag
			{
			public:
				static const int InvalidPosition = -1;

				Bag();
				Bag(uint8 bagNo, uint32 size, int isBankBag);
				~Bag();
				Bag& operator=(const Bag& in) {
					enabled = in.enabled;
					items = in.items;
					bagNo_ = in.bagNo_;
					isBankBag_ = in.isBankBag_;
					return *this;
				}
				Bag& operator=(Bag&& in) = default;
				void Clear();

				bool SetItem(uint32 pos, const Common::ItemEx& item);
				bool AddItem(uint32 pos, uint16 count);
				bool DropItem(uint32 pos, uint16 count, Common::ItemEx* droppedItem);
				void DeleteItem(uint32 pos);

				const Common::ItemEx& GetItem(uint32 pos) const;

				uint32 GetCount(const Common::ItemEx::ICountCondition& condition) const;
				//uint32 GetBagSlots() const;
				int FindItem(const Common::ItemEx::IFindCondition& condition, uint32 beginPosition = 0) const; ///< @return InvalidPosition if the item could not be found or returns index in items
				int FindPetItem(XRated::GlobalSerial itemSerial) const;
				//bool IsStackable(uint32 itemHash, uint32 count, uint32 pos);
				uint32 DeleteItems(uint32 itemHash, uint16 count); ///@ returns total deleted item count
				void AllItems(std::vector< Common::ItemSlotEx >& allItems);
				void AllItems(std::vector< XRated::ItemSlot >& allItems);
				const std::vector<Common::ItemEx>& GetAllItemSlots();
				bool SetItemInstance(uint32 pos, InstanceEx instance);
				const Common::ItemEx* ItemInfoChange(uint8 position, const Database::Info::ItemInfo* info);
				void StageItemList(std::vector<SlotPos>& removeList);
				bool IsEnabled() const { return enabled; }
				void SetEnable(bool enable) { enabled = enable; }
				uint32 GetBagSize() const { return items.size(); } ///@ returns the number of slots in a bag
				DateTime ExpireDate;

			private:
				bool enabled;
				std::vector< Common::ItemEx > items;
				uint8 bagNo_;
				int isBankBag_;

				mutable std::mutex cs;
			};


		}
	}
}