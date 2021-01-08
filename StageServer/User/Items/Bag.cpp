#include "Bag.h"

namespace Lunia {
	namespace XRated {
		namespace StageServer {

			const Database::Info::ItemInfo* GetItemInfo(uint32 itemHash)
			{
				return Database::DatabaseInstance().InfoCollections.Items.Retrieve(itemHash);
			}

			const Database::Info::ItemInfo* GetItemInfo(const wchar_t* itemID)
			{
				return Database::DatabaseInstance().InfoCollections.Items.Retrieve(itemID);
			}

			Bag::Bag()
				: bagNo_(0), isBankBag_(false), enabled(false)
			{
			}
			Bag::Bag(uint8 bagNo, uint32 size, int isBankBag)
				: bagNo_(bagNo), isBankBag_(isBankBag), enabled(false)
			{
				if (size <= 0)
				{
					LoggerInstance().Warn("Bag::Bag() size({}) <= 0", size);
					throw Exception(L"Bag::Bag() - size({}) <= 0", size);
				}

				//slots.assign(size, Slot());
				items.assign(size, Common::ItemEx());

				ExpireDate = DateTime(0, 0, 0, 0, 0, 0); // to make expire immediately
			}


			Bag::~Bag()
			{
			}


			void Bag::Clear()
			{
				AutoLock lock(cs);

				for (std::vector< Common::ItemEx >::iterator i = items.begin(), end = items.end(); i != end; ++i)
					i->Info = NULL;

				enabled = false;
				ExpireDate = DateTime(0, 0, 0, 0, 0, 0); // to make expire immediately		
			}


			bool Bag::SetItem(uint32 pos, const Common::ItemEx& item)
			{
				AutoLock lock(cs);

				if (pos >= items.size()) return false; // out of range
				items[pos] = item;
				return true;
			}

			bool Bag::DropItem(uint32 pos, uint16 count, Common::ItemEx* droppedItem)
			{
				AutoLock lock(cs);

				Common::ItemEx& item = items.at(pos);
				if (item.Info == NULL || count == 0) return false;
				if (item.StackedCount < count) return false;

				item.StackedCount = item.StackedCount - count;
				if (droppedItem != NULL) {
					*droppedItem = item;
				}
				if (item.StackedCount == 0)
					item.Info = NULL;
				return true;
			}

			bool Bag::AddItem(uint32 pos, uint16 count)
			{
				AutoLock lock(cs);

				Common::ItemEx& item = items.at(pos);
				if (item.Info == NULL || count == 0) return false;
				if (item.StackedCount + count > item.Info->MaxCnt) return false;
				item.StackedCount = item.StackedCount + count;
				return true;
			}

			void Bag::DeleteItem(uint32 pos)
			{
				AutoLock lock(cs);

				Common::ItemEx& item = items.at(pos);
				item.Info = NULL;
			}


			const Common::ItemEx& Bag::GetItem(uint32 pos) const
			{
				AutoLock lock(cs);

				return items.at(pos);
			}



			uint32 Bag::GetCount(const Common::ItemEx::ICountCondition& condition) const
			{
				AutoLock lock(cs);

				uint32 count(0);
				for (uint32 i = 0; i < items.size(); ++i)
					count += condition(items[i]);
				return count;
			}

			int Bag::FindItem(const Common::ItemEx::IFindCondition& condition, uint32 beginPosition) const
			{
				AutoLock lock(cs);

				for (uint32 i = beginPosition; i < items.size(); ++i)
					if (condition(items[i])) return i;
				return InvalidPosition;
			}
			int Bag::FindPetItem(XRated::GlobalSerial itemSerial) const
			{
				AutoLock lock(cs);

				for (uint32 i = 0; i < items.size(); ++i) {
					const Common::ItemEx& item = items.at(i);
					if ((item.Info != NULL) && (item.Serial == itemSerial) && (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(item.Info->Hash) == true)) {
						return i;
					}
				}
				return InvalidPosition;
			}


			uint32 Bag::DeleteItems(uint32 itemHash, uint16 count)
			{
				AutoLock lock(cs);

				uint32 deleted(0);
				for (std::vector< Common::ItemEx >::iterator i = items.begin(), end = items.end(); i != end; ++i)
				{
					if (i->Info == NULL || i->Info->Hash != itemHash) continue;
					if (i->StackedCount <= count)
					{
						deleted += i->StackedCount;
						count = count - i->StackedCount;
						i->Info = NULL;
					}
					else
					{
						deleted += count;
						i->StackedCount = i->StackedCount - count;
						count = 0;
					}
					if (count == 0) break;
				}
				return deleted;
			}


			void Bag::AllItems(std::vector< Common::ItemSlotEx >& allItems)
			{
				AutoLock lock(cs);

				for (size_t i = 0; i < items.size(); ++i)
				{
					const Common::ItemEx& item = items[i];
					if (item.Info == NULL) continue;

					Common::ItemSlotEx itemSlotEx;
					itemSlotEx.ItemInfo = item.Info;
					itemSlotEx.Id = item.Info->Hash;
					itemSlotEx.serial = item.Serial;
					itemSlotEx.Stacked = item.StackedCount;
					itemSlotEx.Position.Bag = bagNo_;
					itemSlotEx.Position.Position = i;
					itemSlotEx.IsBank = isBankBag_;
					itemSlotEx.InstanceEx = item.InstanceEx;

					allItems.push_back(itemSlotEx);
				}
			}


			void Bag::AllItems(std::vector< XRated::ItemSlot >& allItems)
			{
				AutoLock lock(cs);

				for (size_t i = 0; i < items.size(); ++i)
				{
					const Common::ItemEx& item = items[i];
					if (item.Info == NULL) continue;

					XRated::ItemSlot itemSlot;
					itemSlot.Id = item.Info->Hash;
					itemSlot.Stacked = item.StackedCount;
					itemSlot.Position.Bag = bagNo_;
					itemSlot.Position.Position = i;
					itemSlot.InstanceEx = item.InstanceEx;

					allItems.push_back(itemSlot);
				}
			}
			const std::vector<Common::ItemEx>& Bag::GetAllItemSlots()
			{
				AutoLock lock(cs);

				return items;
			}


			bool Bag::SetItemInstance(uint32 pos, InstanceEx instance)
			{
				AutoLock lock(cs);

				items[pos].InstanceEx = instance;

				return true;
			}
			const Common::ItemEx* Bag::ItemInfoChange(uint8 position, const Database::Info::ItemInfo* info)
			{
				if (items.size() <= static_cast<size_t>(position)) {
					return NULL;
				}
				items[position].Info = info;
				return &items[position];
			}


			void Bag::StageItemList(std::vector<SlotPos>& removeList)
			{
				AutoLock lock(cs);

				for (int pos = 0; pos < static_cast<int>(items.size()); ++pos)
				{
					if (items[pos].Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::STAGEITEM)
						removeList.push_back(SlotPos(bagNo_, pos));
				}
			}
		}
	}
}