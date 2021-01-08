#include <StageServer/User/User.h>
#include "Items.h"
#include <Core/Utils/ConfigReader.h>
#include <StageServer/Services/ItemSerial.h>
#include <Core/ErrorDefinition.h>
#include <iostream>
#define ALLM_ERROR(message) LoggerInstance().Error message //Too Lazy to replace all.
#define ALLM_IMPORTANT(message) LoggerInstance().Warn message
#define ALLM_WARNING(message) LoggerInstance().Warn message
#define ALLM_INFO(message) LoggerInstance().Info message
#define ALLM_EXCEPTION(message) LoggerInstance().Exception message
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			void Items::ChangedItems::Clear()
			{
				Insert.clear();
				Update.clear();
				Delete.clear();
			}
			/* static helper */

			//bool Items::IsExpired(const Database::Info::ItemInfo* info, int64 instance)
			//{
			//	if (Database::Enchant::IsEnchantable(info)) return false; /* no expiration for enchantable items */

			//	Database::Enchant::NormalBitfields i(instance);
			//	return i.IsExpired();
			//}

			bool Items::IsImportantToLog(const Database::Info::ItemInfo* info)
			{
				if ((info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CASH)
					|| (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USE)
					|| (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USEEX)
					|| (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::PASSIVE)
					|| (info->MaxCnt == 1))
					return true;
				return false;
			}

			Items::Items(IEventListener& listener) : player(NULL), event(listener)
				, inventory(XRated::Constants::MaxOfInventoryBags, XRated::Constants::MaxOfSlotsInInventoryBag, false)
				, bank(XRated::Constants::MaxOfBankBags, XRated::Constants::MaxOfSlotsInBankBag, true)
				, petsInventory((User*)&listener)
			{
			}

			Items::~Items()
			{
			}

			void Items::SetPlayer(XRated::Logic::Player* player)
			{
				AutoLock lock(cs);
				this->player = player;

				itemRequirement.Clear();

				if (player == NULL) return;
			}


			void Items::Clear()
			{
				AutoLock lock(cs);
				player = NULL;

				inventory.Clear();
				equip.Clear();
				bank.Clear();
				originSlots.Clear();

				itemRequirement.Clear(); // <- equipRequirementQueue.clear();
			}

			int Items::SetItemList(const json& Bags, const json& Items)
			{
				Protocol::FromServer::BagStates bagStatePacket;

				AutoLock lock(cs); // items lock
				/* initial bags */
				bank.Init();
				inventory.Init();
				equip.Clear();

				originSlots.Clear();

				std::vector<uint8> defaultBags;
				Database::DatabaseInstance().InfoCollections.BagInfos.GetDefaultBag(defaultBags);
				for (size_t index = 0; index < defaultBags.size(); ++index)
				{
					inventory.RenewBag(defaultBags[index]); // default invenotry
				}

				Database::DatabaseInstance().InfoCollections.BagInfos.GetDefaultBank(defaultBags);
				for (size_t index = 0; index < defaultBags.size(); ++index)
				{
					bank.RenewBag(defaultBags[index]); // default invenotry
				}

				if (Bags.is_null())
					return false;
				//Check the reason why we have two tables doing the same damn thing.
				Protocol::FromServer::BagStates sendPacket;
				//Bags Character
				for (auto& x : Bags["bags"])
					if (!x["isBank"].get<bool>())
						sendPacket.Bags.push_back(XRated::BagState(x["bagNumber"].get<int>(), x["expireDate"].get<std::string>()));

				//Bags Bank
				for (auto& x : Bags["bankBags"])
					sendPacket.BankBags.push_back(XRated::BagState(x["bagNumber"].get<int>(), x["expireDate"].get<std::string>()));

				Send(sendPacket);
				/* initialize bags */
				for (auto& bag : Bags["bags"])
				{
					XRated::BagState state;

					bag.at("bagNumber").get_to(state.BagNumber);
					bag.at("expireDate").get_to(state.ExpireDate);

					inventory.RenewBag(state.BagNumber, state.ExpireDate);
					state.Expired = !(inventory.IsEnable(state.BagNumber));
					bagStatePacket.Bags.push_back(state);
				}

				for (auto& bag : Bags["bankBags"])
				{
					XRated::BagState state;
					bag.at("bagNumber").get_to(state.BagNumber);
					bag.at("expireDate").get_to(state.ExpireDate);

					bank.RenewBag(state.BagNumber, state.ExpireDate);
					state.Expired = !(bank.IsEnable(state.BagNumber));
					bagStatePacket.BankBags.push_back(state);
				}
				event.ItemsDirectSend(bagStatePacket);


				/* initialize items */
				int expired = 0;

				if (Items.empty() || Items.is_null()) // empty items
					return 0;

				Protocol::FromServer::ListItem listitem;
				std::map<int/*bagType*/, std::vector<ItemSlot> > invenItems;
				std::set<GlobalSerial> serials; // buffer to check item duplication
				for (auto& x : Items)
				{
					Common::ItemEx	item;
					int isBankItem = false;
					uint32 id = x.at("itemHash").get<uint32>();
					uint32 pos = x.at("positionNumber").get<uint32>();
					uint32 bag = x.at("bagNumber").get<uint32>();
					x.at("stackedCount").get_to(item.StackedCount);
					x.at("instance").get_to(item.Serial);
					x.at("instance").get_to(item.InstanceEx.Instance);
					x.at("itemExpire").get_to(item.InstanceEx.ExpireDate);

					item.Info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(id);
					if (item.Info == NULL || item.StackedCount == 0)
					{
						ALLM_WARNING((L"invalid item information (hash:{}, count:{})", id, item.StackedCount));
						continue;
					}

#ifdef BAN_DUPLICATED_ITEM_SERIAL
					/* check duplication */
					if (item.Info->MaxCnt == 1 && item.Serial) // items which serial should not be changed and is not from manual(administration or so)
					{
						if (serials.insert(item.Serial).second == false) // the set already contained an element
						{
							ALLM_ERROR((L"duplicated item({}) detected({}) on a user({})", item.Info->Hash, item.Serial, event.GetName().c_str()));

							// arcrus. Log duplicated item, and remove old one.

							// Item Find Condition Interface
							struct Condition : public Common::ItemEx::IFindCondition
							{
								uint64 Serial;
								Condition(uint64 serial) : Serial(serial) {}
								bool operator()(const Common::ItemEx& info) const { return (info.Serial == Serial); }
							};

							const Common::ItemEx* oldItem = NULL;
							bool isOldItemPositionBank = false;
							ItemPosition oldItemPosition = FindItem(Condition(item.Serial), true /* include equip */);

							if (oldItemPosition == ItemPosition::Invalid)
							{
								oldItemPosition = FindBankItem(Condition(item.Serial));
								if (oldItemPosition != ItemPosition::Invalid)
								{
									isOldItemPositionBank = true;
									oldItem = GetItem(oldItemPosition.Bag, oldItemPosition.Position, true);
								}
							}
							else {
								oldItem = GetItem(oldItemPosition.Bag, oldItemPosition.Position);
							}

							if (oldItem == NULL) {
								event.RemoveDuplicatedItem(0, 0, static_cast<uint8>(bag), static_cast<uint8>(pos), 0, 0, isBankItem, false);

								std::wstring message =
									fmt::format(L"[SetItemList]Removed : Character Name = {}, ItemHash({}), ItemSerial({}), ItemBag({}), ItemPos({}), StackedCount({}), Instance({}), ExpireDate({}), isBank({})"
										, event.GetName().c_str()
										, id, item.Serial, bag, pos
										, static_cast<uint32>(item.StackedCount)
										, item.InstanceEx.Instance
										, item.InstanceEx.ExpireDate.ToString()
										, isBankItem
									);

								//DuplicatedItemLoggerInstance().Write(message.c_str());

								message = L"Cannot found duplicated item.";
								//DuplicatedItemLoggerInstance().Write(message.c_str());

							}
							else {
								if ((oldItemPosition.Bag == bag) && (oldItemPosition.Position == pos) && (isOldItemPositionBank == ((isBankItem) ? true : false)))
								{
									event.RemoveDuplicatedItem(id, item.Serial, static_cast<uint8>(bag), static_cast<uint8>(pos)
										, item.StackedCount, item.InstanceEx, isBankItem, true /*insert again after delete it*/);


									std::wstring message;

									message =
										fmt::format(L"[SetItemList]Removed : Character Name = {}, ItemHash({}), ItemSerial({}), ItemBag({}), ItemPos({}), StackedCount({}), Instance({}), ExpireDate({}), isBank({})"
											, event.GetName().c_str()
											, oldItem->Info->Hash, oldItem->Serial, bag, pos
											, static_cast<uint32>(oldItem->StackedCount)
											, oldItem->InstanceEx.Instance
											, oldItem->InstanceEx.ExpireDate.ToString()
											, isBankItem
										);
									//DuplicatedItemLoggerInstance().Write(message.c_str());

									message =
										fmt::format(L"[SetItemList]Inserted : Character Name = {}, ItemHash({}), ItemSerial({}), ItemBag({}), ItemPos({}), StackedCount({}), Instance({}), ExpireDate({}), isBank({})"
											, event.GetName().c_str()
											, id, item.Serial, bag, pos
											, static_cast<uint32>(item.StackedCount)
											, item.InstanceEx.Instance
											, item.InstanceEx.ExpireDate.ToString()
											, isBankItem
										);
									//DuplicatedItemLoggerInstance().Write(message.c_str());

								}
								else
								{
									event.RemoveDuplicatedItem(0, 0, static_cast<uint8>(bag), static_cast<uint8>(pos), 0, 0, isBankItem, false);

									std::wstring message;

									message =
										fmt::format(L"[SetItemList]Removed : Character Name = {}, ItemHash({}), ItemSerial({}), ItemBag({}), ItemPos({}), StackedCount({}), Instance({}), ExpireDate({}), isBank({})"
											, event.GetName().c_str()
											, id, item.Serial, bag, pos
											, static_cast<uint32>(item.StackedCount)
											, item.InstanceEx.Instance
											, item.InstanceEx.ExpireDate.ToString()
											, isBankItem
										);
									//DuplicatedItemLoggerInstance().Write(message.c_str());

									message =
										fmt::format(L"[SetItemList]Duplicated : Character Name = {}, ItemHash({}), ItemSerial({}), ItemBag({}), ItemPos({}), StackedCount({}), Instance({}), ExpireDate({}), isBank({})"
											, event.GetName().c_str()
											, oldItem->Info->Hash, oldItem->Serial, oldItemPosition.Bag, oldItemPosition.Position
											, static_cast<uint32>(oldItem->StackedCount)
											, oldItem->InstanceEx.Instance
											, oldItem->InstanceEx.ExpireDate.ToString()
											, isBankItem
										);
									//DuplicatedItemLoggerInstance().Write(message.c_str());
								}

							}


							return -1;

						}
					}
#endif

					if (isBankItem) {
						originSlots.SetBankItem(bag, pos, item);
					}
					else {
						if (Database::DatabaseInstance().InfoCollections.BagInfos.EqualBagType(Database::Info::BagInfos::BagType::Equip, bag)) {
							originSlots.SetEquip(bag, pos, item);
						}
						else {
							originSlots.SetInventoryItem(bag, pos, item);
						}
					}

					/* remove stageitem */
					if (item.Info->Attributes.Attr & (1 << 8))
					{
						Log("StageItem Removed", item);
						continue;
					}

					/* check expiration */
					if (Database::Enchant::IsExpired(item.Info, item.InstanceEx) && !(item.Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::EXISTWHENEXPIRED))
					{
						Log("expireditem", item);
						++expired;
						continue;
					}

					/* clipping stack count - actually this should not be happended. */
					if (item.StackedCount > static_cast<uint32>(item.Info->MaxCnt))
					{
						ALLM_IMPORTANT((L"Items::SetItemList() item.Id({}) count({}) > pItemInfo->MaxCnt({})", id, item.StackedCount, item.Info->MaxCnt));
						item.StackedCount = static_cast<uint16>(item.Info->MaxCnt);
					}

					if (!isBankItem)
					{
						bool pushed;
						if (Database::DatabaseInstance().InfoCollections.BagInfos.EqualBagType(Database::Info::BagInfos::BagType::Equip, bag)) // ����/���/�Ǽ��縮
						{
							//pushed = equip.PushItems(pos, item);
							pushed = equip.SetItems(bag, pos, item);
						}
						else
						{
							pushed = inventory.SetItem(bag, pos, item);
						}

						if (!pushed)
							continue;

						if (!Database::DatabaseInstance().InfoCollections.BagInfos.EqualBagType(Database::Info::BagInfos::BagType::Equip, bag) || equip.IsCurrentEquipSet(bag, pos))
						{
							XRated::ItemSlot itemslot;
							itemslot.Id = id;
							if (equip.IsCurrentEquipSet(bag, pos))
							{
								itemslot.Position.Bag = 0;
							}
							else
							{
								itemslot.Position.Bag = bag;
							}
							itemslot.Position.Position = static_cast<uint8>(pos);
							itemslot.Stacked = item.StackedCount;
							itemslot.InstanceEx = item.InstanceEx;

							invenItems[Database::DatabaseInstance().InfoCollections.BagInfos.GetBagType(bag)].push_back(itemslot);
						}
					}
				}

				std::map<int, std::vector<ItemSlot> >::const_iterator itemIter = invenItems.begin();
				std::map<int, std::vector<ItemSlot> >::const_iterator itemEndIter = invenItems.end();
				for (; itemIter != itemEndIter; ++itemIter)
				{
					listitem.listitem = itemIter->second;
					event.ItemsDirectSend(listitem);
				}

				return expired;
			}

			uint32 Items::GetBagsSlots(bool isBank) {
				std::vector<uint8> Bags;
				uint32 count(0);
				if (isBank) {
					Bags.resize(bank.GetNumberOfBags());
					bank.GetBags(Bags);
					for (int i = 0; i < Bags.size(); i++) {
						Database::Info::BagInfos::BagType bagType =
							Database::DatabaseInstance().InfoCollections.BagInfos.GetBankType(Bags[i]);
						if (bank.IsEnable(Bags[i]) &&
							bank.GetBagExpireDate(Bags[i]) == DateTime::Infinite &&
							bagType == Database::Info::BagInfos::BagType::Normal) {
							count++;
						}
					}
				}
				else {
					Bags.resize(inventory.GetNumberOfBags());
					inventory.GetBags(Bags);
					for (int i = 0; i < Bags.size(); i++) {
						Database::Info::BagInfos::BagType bagType =
							Database::DatabaseInstance().InfoCollections.BagInfos.GetBagType(Bags[i]);
						if (inventory.IsEnable(Bags[i]) &&
							inventory.GetBagExpireDate(Bags[i]) == DateTime::Infinite &&
							bagType == Database::Info::BagInfos::BagType::Normal) {
							count++;
						}
					}
				}
				return count;
			}

			void Items::ResetExpiredBag()
			{
				Protocol::FromServer::ChangedBagStates bagStatePacket;
				XRated::BagState state;

				{
					std::vector<uint8> bankBags;
					bank.GetBags(bankBags);
					uint16 bagCount = bankBags.size();
					for (uint16 i = 0; i < bagCount; ++i)
					{
						if (bank.IsEnable(bankBags[i]) == true) {
							state.BagNumber = bankBags[i];
							state.ExpireDate = bank.GetBagExpireDate(state.BagNumber);
							bank.ResetEnable(state.BagNumber);
							state.Expired = !(bank.IsEnable(state.BagNumber));
							//enable���¿��� expired�Ȱ͵鸸 Ŭ���̾�Ʈ���� �����ش�.
							if (state.Expired == true) {
								bagStatePacket.BankBags.push_back(state);
							}
						}
					}
				}
				{
					std::vector<uint8> inventoryBags;
					inventory.GetBags(inventoryBags);
					uint16 bagCount = inventoryBags.size();
					for (uint16 i = 1; i < bagCount; ++i)
					{
						if (inventory.IsEnable(inventoryBags[i]) == true) {
							state.ExpireDate = inventory.GetBagExpireDate(inventoryBags[i]);
							inventory.ResetEnable(inventoryBags[i]);
							state.Expired = !(inventory.IsEnable(inventoryBags[i]));
							state.BagNumber = inventoryBags[i];
							if (state.Expired == true) {
								bagStatePacket.Bags.push_back(state);
							}
						}
					}
				}
				event.ItemsDirectSend(bagStatePacket);
			}

			bool Items::IsEnable(bool isBank, int bagNumber) const
			{
				if (bagNumber > 0)
				{
					const Inventory& inv = isBank ? bank : inventory;
					return inv.IsEnable(bagNumber);
				}
				// �����ȣ�� 0�̸� ����. �ϴ� ��.
				else if (bagNumber == 0)
				{
					return true;
				}

				return	false;
			}

			DateTime Items::RenewBag(bool isBank, int bagNumber, int days)
			{
				Inventory& inv = isBank ? bank : inventory;
				if (days == 0) /* means no expiration */
				{
					inv.RenewBag(bagNumber);
					return DateTime::Infinite;
				}

				DateTime expireDate = inv.GetBagExpireDate(bagNumber);
				DateTime now = DateTime::Now();
				if (expireDate > now) // just expand from the expiration date
					expireDate.Add(DateTime::Unit::Day, days);
				else
					expireDate = now.Add(DateTime::Unit::Day, days);

				inv.RenewBag(bagNumber, expireDate);
				return expireDate;
			}

			DateTime Items::GetExpireDate(bool isBank, int bagNumber) const
			{
				const Inventory& inv = isBank ? bank : inventory;
				return inv.GetBagExpireDate(bagNumber);
			}

			uint16 Items::GetNumberOfBags(bool isBank) const
			{
				const Inventory& inv = isBank ? bank : inventory;
				return inv.GetNumberOfBags();
			}

			int Items::GetItemCount(uint32 itemHash) const
			{
				return inventory.GetItemCount(itemHash);
			}

			int Items::GetItemCountInPetInventory(uint32 itemHash, bool onlyAppear) const
			{
				return petsInventory.GetItemCount(itemHash, onlyAppear);
			}

			int Items::GetItemCountInPetInventory(const Database::Info::ItemInfo::IFindCondition& condition, bool onlyAppear) const
			{
				return petsInventory.GetItemCount(condition, onlyAppear);
			}

			uint32 Items::GetItemCount(const Database::Info::ItemInfo::IFindCondition& condition, bool includeEquipped) const
			{
				class Condition : public Common::ItemEx::ICountCondition
				{
				public:
					Condition(const Database::Info::ItemInfo::IFindCondition& condition) : condition(condition) {}
					uint32 operator()(const Common::ItemEx& rhs) const
					{
						if (rhs.Info && condition(*rhs.Info)) return rhs.StackedCount;
						return 0;
					}

				private:
					const Database::Info::ItemInfo::IFindCondition& condition;
				};


				uint32 count(0);

				/* count equipment */
				if (includeEquipped)
				{
					std::vector<Common::ItemEx> equipped;
					equip.GetEquip(equipped);
					for (std::vector<Common::ItemEx>::const_iterator i = equipped.begin(); i != equipped.end(); ++i)
						if (i->Info && condition(*(i->Info))) ++count;
				}

				/* count inventory */
				count += inventory.GetCount(Condition(condition));

				return count;
			}

			ItemPosition Items::FindItem(const Common::ItemEx::IFindCondition& condition, bool includeEquipped) const
			{
				if (includeEquipped)
				{
					std::vector<Common::ItemEx> equipped;
					equip.GetEquip(equipped);
					for (std::size_t i = 0; i < equipped.size(); ++i)
						if (condition(equipped[i])) return ItemPosition(0, i);
				}

				ItemPosition pos(inventory.FindItemPosition(condition));
				if (pos == ItemPosition::Invalid)
					return ItemPosition::Invalid;
				return pos;
			}

			ItemPosition Items::FindBankItem(const Common::ItemEx::IFindCondition& condition) const
			{
				ItemPosition pos = bank.FindItemPosition(condition);
				return pos;
			}



			Common::PetItemSlotEx Items::FindItemInPetInventory(uint32 itemHash, bool onlyAppear) const
			{
				return petsInventory.FindItem(itemHash, onlyAppear);
			}


			const Common::ItemEx* Items::ItemInfoChangeToInventory(uint8 bag, uint8 position, const Database::Info::ItemInfo* info)
			{
				AutoLock lock(cs);
				const Common::ItemEx* item = inventory.ItemInfoChange(bag, position, info);
				if (item != NULL) {
					Protocol::FromServer::AcquireItem acquireItem;
					acquireItem.ItemId = item->Info->Hash;
					acquireItem.Bag = static_cast<uint16>(bag);
					acquireItem.Pos = static_cast<uint16>(position);
					acquireItem.StackedCount = item->StackedCount;
					acquireItem.instanceEx = item->InstanceEx;

					Send(acquireItem);
				}
				return item;
			}
			ItemPosition Items::FindPetItem(XRated::GlobalSerial itemSerial)
			{
				ItemPosition pos(inventory.FindPetItem(itemSerial));
				if (pos != ItemPosition::Invalid)
				{
					return pos;
				}

				return equip.FindItem(itemSerial);
			}

			bool Items::ChangeItemInstance(uint32 bag, uint32 pos, InstanceEx instance)
			{
				//if(bag < 1)
				//	return false;
				const Common::ItemEx* item;
				if (bag < 1)
				{
					item = equip.GetItem(pos);
				}
				else
				{
					item = inventory.GetItem(bag, pos);
				}

				/* Item Instance Logging */
				/*Logger().Write(IAdmLogger::ImportantLogger, "ItemInstance-Changed", event.GetName(), Http::Logger::Parameter()
					<< bag << pos << item->InstanceEx.Instance << item->InstanceEx.ExpireDate.ToString()
					<< instance.Instance << instance.ExpireDate.ToString());*/

				if (bag < 1)
					return equip.SetItemInstance(pos, instance);

				return inventory.SetItemInstance(bag, pos, instance);
			}

			int Items::SetPetsInventoryItem(const std::string& items, std::vector< XRated::Pet >& pets)
			{
				petsInventory.Init();
				int expired;
				Protocol::FromServer::ListPetItem	packet;
				expired = petsInventory.SetItemList(items, pets, packet);
				//packet send.
				event.PetInventoryDirectSend(packet);
				return expired;
			}

			void Items::AddInventory(const XRated::Pet& pet)
			{
				petsInventory.AddInventory(pet);
			}


			bool Items::IsItemInPetInventory(const GlobalSerial petSerial)
			{
				return petsInventory.IsItemInPetInventory(petSerial);
			}

			bool Items::CheckInventoryFull(void)
			{
				ItemPosition pos;
				pos = inventory.FindEmptySlot(NULL);

				if (pos == ItemPosition::Invalid)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::InventoryFull;
					error.errorstring = Errors::ToString(Errors::InventoryFull);
					Send(error);
					return false;
				}

				return true;
			}
			bool Items::SortInventory()
			{
				return false;
				//bool isItemChanged=false;
				////XRated::ItemPosition to = inventory.FindEmptySlot();
				//XRated::ItemPosition to = XRated::ItemPosition(0,0);
				//XRated::ItemPosition from = XRated::ItemPosition(0,0);
				//while( to != XRated::ItemPosition::Invalid)
				//{
				//	++from.Position;
				//	from = inventory.GetUnsortedItemPosition( from );
				//	if( from == XRated::ItemPosition::Invalid )
				//	{
				//		//���� ��
				//		return isItemChanged;
				//	}

				//	const Common::ItemEx* item = inventory.GetItem(from);
				//	if( item->Info->MaxCnt > item->StackedCount )
				//	{
				//		XRated::ItemPosition toStack = inventory.FindStackableSlot(item->Info, item->Instance, item->StackedCount);
				//		if( toStack != XRated::ItemPosition::Invalid )
				//		{
				//			if( from.Bag != toStack.Bag || from.Position != toStack.Position )
				//			{
				//				StackItems(from.Bag, from.Position , toStack.Bag, toStack.Position, item->StackedCount );
				//				isItemChanged=true;
				//				continue;
				//			}
				//		}
				//		else
				//			to = inventory.FindEmptySlot();
				//	}
				//	else
				//	{
				//		to = inventory.FindEmptySlot();
				//	}

				//	if( to != XRated::ItemPosition::Invalid )
				//	{
				//		if( to.Bag < from.Bag || (to.Bag == from.Bag && to.Position < from.Position ))
				//		{
				//			MoveItems(from.Bag, from.Position , to.Bag, to.Position);
				//			isItemChanged=true;
				//		}
				//	}
				//	
				//}
				//
				//return isItemChanged;
			}

			const Common::ItemEx* Items::GetItem(uint32 bag, uint32 pos, bool isBank) const
			{
				if (isBank == false)
				{
					if (bag == 0)
						return equip.GetItem(pos);
					return inventory.GetItem(bag, pos);
				}
				return bank.GetItem(bag, pos);
			}

			const Common::ItemEx* Items::GetItemInPetInven(const XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType type, uint8 pos) const
			{
				return petsInventory.GetItem(petSerial, type, pos);
			}

			//uint8 Items::GetStackedCount(uint32 bag, uint32 pos) const
			//{
			//	if (bag==0) return 0;
			//	return static_cast<uint8>(inventory.GetItemCount(bag-1, pos));
			//}

			void Items::SetItem(uint32 bag, uint32 pos, const Common::ItemEx& item, bool sendPacket)
			{
				if (bag == 0)
				{
					equip.DeleteItem(pos);
					equip.PushItems(pos, item);
				}
				else
				{
					inventory.SetItem(bag, pos, item);
					if (sendPacket)
					{
						Protocol::FromServer::AcquireItem acquireitem;
						acquireitem.ItemId = item.Info->Hash;
						acquireitem.Bag = static_cast<uint16>(bag);
						acquireitem.Pos = static_cast<uint16>(pos);
						acquireitem.StackedCount = item.StackedCount;
						acquireitem.instanceEx = item.InstanceEx;
						Send(acquireitem);
					}
				}
			}

			bool Items::ChangeEquipSet(uint8 set)
			{
				//index�� ����������
				return equip.ChangeEquipSet(Database::DatabaseInstance().InfoCollections.BagInfos.SetIndexToBagNumber(set));
			}
			bool Items::SwapEquipment(uint8 set)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				uint8 oldEquipSet = equip.GetCurrentEquipSet();

				if (ChangeEquipSet(set))
				{
					// ������ ��û�� ť�� ����
					if (!itemRequirement.RequireSwapEquip(set))
					{
						ALLM_ERROR((L"Items::SwapEquipment() - Error : SwapEquipment() == false"));
						return false;
					}

					std::vector< EquippedItem > newEquipments;
					equip.GetNormalEquipments(newEquipments);
					// ������ ��û
					//uint32 equipSize=XRated::Constants::Equipment::Cnt;
					//for(uint32 i=0; i<equipSize; ++i)
					//{
					//	if( i >= XRated::Constants::Equipment::CashBody )
					//		continue;

					//	EquippedItem newItem;
					//	const Common::ItemEx* item=equip.GetItem(i);
					//	if (item==NULL) 
					//	{
					//		newItem.itemHash = 0;
					//		newItem.where = static_cast<XRated::Constants::Equipment>(i);
					//		newItem.instance = 0;
					//	}
					//	else
					//	{
					//		newItem.itemHash = item->Info->Hash;
					//		newItem.where = static_cast<XRated::Constants::Equipment>(i);
					//		newItem.instance = item->Instance;
					//	}
					//	newEquipments.push_back( newItem );
					//}

					if (!event.EquipmentSwap(newEquipments))
					{
						ALLM_ERROR((L"Items::SwapEquipment() - Error : event.SwapEquip() == false"));
						itemRequirement.Clear();

						Protocol::FromServer::EquipmentSwapped packet;
						packet.Result = Protocol::FromServer::EquipmentSwapped::Error;
						Send(packet);

						ChangeEquipSet(oldEquipSet);

						return false;
					}
				}

				return true;
			}

			bool Items::ChangeCashEquipSet(uint8 set)
			{
				//index�� ����������
				return equip.ChangeCashEquipSet(Database::DatabaseInstance().InfoCollections.BagInfos.SetCashIndexToBagNumber(set));
			}

			bool Items::SwapCashEquipment(uint8 set)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				uint8 oldEquipSet = equip.GetCurrentCashEquipSet();

				if (ChangeCashEquipSet(set))
				{
					if (!itemRequirement.RequireSwapCashEquip(set))
					{
						ALLM_ERROR((L"Items::SwapCashEquipment() - Error : SwapEquipment() == false"));
						return false;
					}

					std::vector< EquippedItem > newEquipments;
					equip.GetCashEquipments(newEquipments);
					// ������ ��û
					//uint32 equipSize=XRated::Constants::Equipment::Cnt;
					//for(uint32 i=0; i<equipSize; ++i)
					//{
					//	if( i < XRated::Constants::Equipment::CashBody && i > XRated::Constants::Equipment::CashEtc2 )
					//		continue;

					//	EquippedItem newItem;
					//	const Common::ItemEx* item=equip.GetItem(i);
					//	if (item==NULL) 
					//	{
					//		newItem.itemHash = 0;
					//		newItem.where = static_cast<XRated::Constants::Equipment>(i);
					//		newItem.instance = 0;
					//	}
					//	else
					//	{
					//		newItem.itemHash = item->Info->Hash;
					//		newItem.where = static_cast<XRated::Constants::Equipment>(i);
					//		newItem.instance = item->Instance;
					//	}
					//	
					//	newEquipments.push_back( newItem );
					//}
					if (!event.CashEquipmentSwap(newEquipments))
					{
						ALLM_ERROR((L"Items::SwapCashEquipment() - Error : event.SwapEquip() == false"));
						itemRequirement.Clear();

						Protocol::FromServer::CashEquipmentSwapped packet;
						packet.Result = Protocol::FromServer::CashEquipmentSwapped::Error;
						Send(packet);

						ChangeCashEquipSet(oldEquipSet);

						return false;
					}
				}

				return true;
			}

			bool Items::MoveItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				// bag : 0 == equip, 1~XRated::Constants::MaxOfInventoryBags == inventory (when require)

				// �κ������� �̵�
				if ((sBag > 0) && (tBag > 0))
				{
					if (!inventory.MoveItems(sBag, sPos, tBag, tPos))
						return false;

					Protocol::FromServer::MoveItem moveitem;
					moveitem.pbag = static_cast<uint16>(sBag);
					moveitem.ppos = static_cast<uint16>(sPos);
					moveitem.nbag = static_cast<uint16>(tBag);
					moveitem.npos = static_cast<uint16>(tPos);
					Send(moveitem);
				}
				else if ((sBag > 0) && (tBag == 0)) // ��� ����(�Ա�) ��û
				{
					/* equip-item validation */
					const Common::ItemEx* item = inventory.GetItem((sBag), sPos);
					if (item == NULL || player == NULL)
					{
						ALLM_ERROR((L"should not happen here"));
						return false;
					}
					if (IsAbleToUse(player, item->Info, event.GetStageInfo()) == false) // client should validate this
					{
						ALLM_ERROR((L"invalid item to equip by limits"));
						return false;
					}

					/*  comment for one-piece
						there're 6 types to be careful.
							1. equipping an one-piece to an empty slot
							2. equipping an one-piece to an equipped slot
							3. equipping an one-piece to an empty slot which is a part of equipped one-piece
							4. unequipping an one-piece to an empty slot
							5. unequipping an one-piece to an normal equippable item
							6. unequipping an one-piece to another one-piece
					*/
					return RequirePutOnEquip(sBag, sPos, tPos);
				}
				// ��� ����(����) ��û
				else if ((sBag == 0) && (tBag > 0))
				{
					return RequireTakeOffEquip(sPos, tBag, tPos);
				}
				else // sBag==0 and tBag==0  ; equipment to equip in case of eqrings and rings
				{
					return RequireEquipToEquip(sPos, tPos);
				}

				return true;
			}




			bool Items::StackItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos, uint32 count)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				// bag : 0 == equip, 1~XRated::Constants::MaxOfInventoryBags == inventory (when require)
				if (sBag == Constants::EquipmentBagNumber || tBag == Constants::EquipmentBagNumber
					|| inventory.StackItems(sBag, sPos, tBag, tPos, count) == false)
				{
					return false;
				}

				Protocol::FromServer::StackItem stackitem;
				stackitem.sbag = static_cast<uint16>(sBag);
				stackitem.spos = static_cast<uint16>(sPos);
				stackitem.tbag = static_cast<uint16>(tBag);
				stackitem.tpos = static_cast<uint16>(tPos);
				stackitem.count = count;
				Send(stackitem);
				return true;
			}


			bool Items::UseItem(uint32 bag, uint32 pos)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				const Common::ItemEx* item(NULL);
				if (Database::DatabaseInstance().InfoCollections.BagInfos.EqualBagType(Database::Info::BagInfos::BagType::Equip, bag))
				{
					item = equip.GetItem(pos);
					if (item == NULL || !(item->Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USE))
					{
						ALLM_ERROR((L"invalid item to use equipment, pos({}, {}) : item - {}", bag, pos, item ? item->Info->Hash : 0));
						return false;
					}
				}
				else
				{
					item = inventory.GetItem((bag), pos);
					if (item == NULL || !(item->Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USE))
					{
						ALLM_ERROR((L"invalid item to use bag, pos({}, {}) : item - {}", bag, pos, item ? item->Info->Hash : 0));
						return false;
					}
				}

				/* use-item validation */
				if (player == NULL || item == NULL)
				{
					ALLM_ERROR((L"not expected"));
					return false;
				}
				if (IsAbleToUse(player, item->Info, event.GetStageInfo()) == false) // client should validate this
				{
					ALLM_ERROR((L"invalid item to use by limits"));
					return false;
				}

				// ������ ��û�� ť�� ����
				if (!itemRequirement.RequireItemUse(bag, pos))
				{
					ALLM_ERROR((L"Items::UseItem() - Error : RequireItemUse() == false"));
					return false;
				}

				// ������ ��û
				if (!event.ItemsUse(item->Info->Hash, bag, pos))
				{
					ALLM_ERROR((L"Items::UseItem() - Error : event.ItemsUse() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::UnableToUseItem;
					Send(error);

					return false;
				}

				return true;
			}

			bool Items::UsePetItem(uint32 bag, uint32 pos)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				const Common::ItemEx* item(NULL);
				if (Database::DatabaseInstance().InfoCollections.BagInfos.EqualBagType(Database::Info::BagInfos::BagType::Equip, bag))
				{
					item = equip.GetItem(pos);
					if (item == NULL)
					{
						ALLM_ERROR((L"invalid item to use bag, pos({}, {}) : item - {}", bag, pos, item ? item->Info->Hash : 0));
						return false;
					}
				}
				else
				{
					item = inventory.GetItem((bag), pos);
					if (item == NULL)
					{
						ALLM_ERROR((L"invalid item to use bag, pos({}, {}) : item - {}", bag, pos, item ? item->Info->Hash : 0));
						return false;
					}
				}

				/* use-item validation */
				if (player == NULL || item == NULL)
				{
					ALLM_ERROR((L"not expected"));
					return false;
				}
				if (IsAbleToUse(player, item->Info, event.GetStageInfo()) == false) // client should validate this
				{
					ALLM_ERROR((L"invalid item to use by limits"));
					return false;
				}

				// ������ ��û�� ť�� ����
				if (!itemRequirement.RequireItemUse(bag, pos))
				{
					ALLM_ERROR((L"Items::UsePetItem() - Error : RequireItemUse() == false"));
					return false;
				}

				// ������ ��û
				if (!event.PetItemUse(item->Info->Hash, bag, pos, item->Serial))
				{
					ALLM_ERROR((L"Items::UsePetItem() - Error : event.PetItemUse() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::UnableToUseItem;
					Send(error);

					return false;
				}

				return true;
			}

			size_t Items::ReagentItemUsed(uint32 skillGroupName)
			{
				size_t removedItemCnt = 0;

				// Skill Group ���
				Database::Info::SkillInfoList::SkillInfo* skillinfo = event.GetSkillInfo(skillGroupName);
				if (skillinfo == NULL)
					return 0;

				size_t size = skillinfo->Reagents.size();
				for (size_t i = 0; i < size; ++i)
				{
					if (inventory.GetItemCount(skillinfo->Reagents[i].Hash) + GetItemCountInPetInventory(skillinfo->Reagents[i].Hash, true) < static_cast<uint32>(skillinfo->Reagents[i].Cnt))
					{ // TODO: make not to happen this. this should not be happened
						ALLM_IMPORTANT((L"reagent item count mismatched skill({}), item({}), current({}), current petInven({}), required({})",
							skillGroupName, skillinfo->Reagents[i].Hash, inventory.GetItemCount(skillinfo->Reagents[i].Hash),
							GetItemCountInPetInventory(skillinfo->Reagents[i].Hash, true), skillinfo->Reagents[i].Cnt));

						/* more logs */
						ALLM_IMPORTANT((L"user({}) ", player->GetName().c_str() ? player->GetName().c_str() : L"NULL"));

						return 0; // throw than return
					}

					uint32 deletedItemCount(0);
					while (deletedItemCount < static_cast<uint32>(skillinfo->Reagents[i].Cnt))
					{
						deletedItemCount += inventory.DeleteItems(skillinfo->Reagents[i].Hash, skillinfo->Reagents[i].Cnt - deletedItemCount);
						deletedItemCount += petsInventory.DeleteItems(skillinfo->Reagents[i].Hash, skillinfo->Reagents[i].Cnt - deletedItemCount, true);
					}

					removedItemCnt += deletedItemCount;
				}

				return removedItemCnt;
			}


			bool Items::RequirePutOnEquip(uint32 invenBag, uint32 invenPos, uint32 equipPos)
			{
				const Common::ItemEx* invenItem = inventory.GetItem(invenBag, invenPos);
				if (invenItem == NULL)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"���� �Ϸ��� ��� ã�� ���߽��ϴ�.";
					Send(error);
					return false;
				}

				uint32 equipItemHash(0);
				const Common::ItemEx* equipItem = equip.GetItem(equipPos);
				if (equipItem == NULL)
				{
					equipItemHash = 0;
				}
				else
				{
					equipItemHash = equipItem->Info->Hash;
					if (invenItem->Info->EquipParts != equipItem->Info->EquipParts)
					{
						ALLM_ERROR((L"invalid trying to equip an item({}) to part {}", invenItem->Info->Hash, invenItem->Info->EquipParts));
						return false;
					}
				}



				///* validation of empty slots for one-piece replacement */
				//int count=0; // counter of the slots which is needed for unequip
				//if (equipItemHash!=0) ++count; // counting basic(representative) equipment

				//const Common::ItemEx* item;
				//for(std::vector<XRated::Constants::EquipParts>::iterator i=invenItemInfo->OnePiece.begin(); i!=invenItemInfo->OnePiece.end(); ++i) // counting additional linked equipment
				//{
				//	item=equip.GetItem( static_cast<uint32>(XRated::Constants::GetEquipPosition(*i)) );
				//	switch( *i )
				//	{
				//		/* ring and earing have two slots */
				//	case XRated::Constants::EquipParts::EARING:
				//	case XRated::Constants::EquipParts::RING:
				//	case XRated::Constants::EquipParts::CASH_EARING:
				//	case XRated::Constants::EquipParts::CASH_RING:
				//		if (item!=NULL) item=equip.GetItem( static_cast<uint32>(XRated::Constants::GetEquipPosition(*i))+1 ); // check both slots are filled
				//		break;
				//	}
				//	if (item!=NULL) ++count;
				//}

				//if (inventory.BlankSlotCount()<static_cast<uint32>(count-1)) // not enough inventory
				//{
				//	/* TODO: make critical (disconnect user) */
				//	return false;
				//}


				///* validation - no equipment should not be duplicated even another one-piece */
				//std::set< XRated::Constants::EquipParts > equipped;
				//for(uint16 pos=0; pos<XRated::Constants::Equipment::Cnt; ++pos)
				//{
				//	const Common::ItemEx* item=equip.GetItem( pos );
				//	if (item==NULL) continue;

				//	if (item->Info->EquipParts!=invenItem->Info->EquipParts) // equipment can be replace if only main equip part exists
				//		equipped.insert(item->Info->EquipParts);
				//	if (item->Info->OnePiece.empty()==false) // one-piece item
				//		equipped.insert(item->Info->OnePiece.begin(), item->Info->OnePiece.end());
				//}
				//if (equipped.find(invenItem->Info->EquipParts)!=equipped.end()) // the equip part exists as a part of another one-piece
				//{
				//	/* TODO ; treat as critical? */
				//	ALLM_ERROR((L"unable to equip there is a part of one-piece"));
				//	return false;
				//}
				//for(std::vector<XRated::Constants::EquipParts>::const_iterator i=invenItem->Info->OnePiece.begin(), end=invenItem->Info->OnePiece.end(); i!=end; ++i) // in case of one-piece item equipment
				//{
				//	if (equipped.find(*i)!=equipped.end()) // onc-piece slot is not empty
				//	{
				//		/* TODO ; treat as critical? */
				//		ALLM_ERROR((L"unable to equip one-piece there is a part of one-piece"));
				//		return false;
				//	}
				//}

				// ������ ��û�� ť�� ����
				if (!itemRequirement.RequireMoveEquip(true, equipItemHash, equipPos, invenItem->Info->Hash, invenPos, invenBag))
				{
					ALLM_ERROR((L"Items::RequireMoveEquip() - Error : RequireMoveEquip() == false"));
					return false;
				}


				// ������ ��û
				if (!event.ItemsEquip(invenItem->Info, invenItem->InstanceEx, equipPos))
				{
					ALLM_ERROR((L"Items::RequirePutOnEquip() - Error : event.ItemsEquip() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::NoResponse;
					Send(error);

					return false;
				}

				return true;
			}

			bool Items::RequireTakeOffEquip(uint32 equipPos, uint32 invenBag, uint32 invenPos)
			{
				const Common::ItemEx* equipItem = equip.GetItem(equipPos);
				const Common::ItemEx* invenItem = inventory.GetItem(invenBag, invenPos);

				if (equipItem == NULL)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"���� �� ��� ã�� ���߽��ϴ�.";
					Send(error);
					return false;
				}

				if (invenItem != NULL)
				{
					return RequirePutOnEquip(invenBag, invenPos, equipPos);
				}

				// ������ ��û�� ť�� ����
				if (!itemRequirement.RequireMoveEquip(false, equipItem->Info->Hash, equipPos, 0, invenPos, invenBag))
				{
					ALLM_ERROR((L"Items::RequireTakeOffEquip() - Error : RequireMoveEquip() == false"));
					return false;
				}

				// ������ ��û
				if (!event.ItemsEquip(NULL, 0, equipPos))
				{
					ALLM_ERROR((L"Items::TakeOffEquip() - Error : event.ItemsEquip() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::NoResponse;
					Send(error);

					return false;
				}

				return true;
			}


			bool Items::RequireDropEquip(uint32 equipPos)
			{
				const Common::ItemEx* equipItem = equip.GetItem(equipPos);

				if (equipItem == NULL)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"���� ��� ã�� ���߽��ϴ�.";
					Send(error);
					return false;
				}

				// ������ ��û �� ť�� ����
				if (!itemRequirement.RequireDropEquip(equipPos))
				{
					ALLM_ERROR((L"Items::RequireDropEquip() - Error : RequireDropEquip() == false"));
					return false;
				}

				// ������ ��û
				if (!event.ItemsEquip(NULL, 0, equipPos))
				{
					ALLM_ERROR((L"Items::RequireDropEquip() - Error : event.ItemsEquip() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::NoResponse;
					Send(error);

					return false;
				}

				return true;
			}

			bool Items::RequireEquipToEquip(uint32 fromEquipPos, uint32 toEquipPos)
			{
				const Common::ItemEx* fromEquipItem = equip.GetItem(fromEquipPos);
				const Common::ItemEx* toEquipItem = equip.GetItem(toEquipPos);

				if ((fromEquipItem == NULL) && (toEquipItem == NULL))
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"���� �Ϸ��� ��� ã�� ���߽��ϴ�.";
					Send(error);
					return false;
				}

				// ������ ��û �� ť�� ����
				if (!itemRequirement.RequireEquipToEquip(fromEquipPos, toEquipPos))
				{
					ALLM_ERROR((L"Items::RequireEquipToEquip() - Error : RequireEquipToEquip() == false"));
					return false;
				}

				// ������ ��û
				if (!event.ItemsEquipToEquip(fromEquipPos, toEquipPos))
				{
					ALLM_ERROR((L"Items::ItemsEquipToEquip() - Error : event.ItemsEquipToEquip() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::NoResponse;
					Send(error);

					return false;
				}

				return true;
			}


			bool Items::IsItemRequirement()
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return true;
				}

				return false;
			}

			bool Items::EquipDirectly(uint32 itemHash, InstanceEx instance, uint64 serial, uint32 equipPos)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				const Database::Info::ItemInfo* bankItem = Database::DatabaseInstance().InfoCollections.Items.Retrieve(itemHash);
				if (bankItem == NULL)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"���� �Ϸ��� ��� ã�� ���߽��ϴ�.";
					Send(error);
					return false;
				}

				uint32 equipItemHash(0);
				const Common::ItemEx* equipItem = equip.GetItem(equipPos);
				if (equipItem)
				{
					equipItemHash = equipItem->Info->Hash;
				}

				/* validation - no equipment should not be duplicated even another one-piece */
				if (Database::Enchant::IsEnchantable(bankItem) && static_cast<Database::Enchant::EnchantBitfields>(instance).IsDestoryed())
				{
					ALLM_ERROR((L"destroied equipment requested - from bank to equip"));
					return	false;
				}

				std::set< XRated::Constants::EquipParts > equipped;
				for (uint16 pos = 0; pos < XRated::Constants::Equipment::Cnt; ++pos)
				{
					const Common::ItemEx* item = equip.GetItem(pos);
					if (item == NULL) continue;

					if (item->Info->EquipParts != bankItem->EquipParts) // equipment can be replace if only main equip part exists
						equipped.insert(item->Info->EquipParts);
					if (item->Info->OnePiece.empty() == false) // one-piece item
						equipped.insert(item->Info->OnePiece.begin(), item->Info->OnePiece.end());
				}
				if (equipped.find(bankItem->EquipParts) != equipped.end()) // the equip part exists as a part of another one-piece
				{
					/* TODO ; treat as critical? */
					ALLM_ERROR((L"unable to equip there is a part of one-piece"));
					return false;
				}
				for (std::vector<XRated::Constants::EquipParts>::const_iterator i = bankItem->OnePiece.begin(), end = bankItem->OnePiece.end(); i != end; ++i) // in case of one-piece item equipment
				{
					if (equipped.find(*i) != equipped.end()) // onc-piece slot is not empty
					{
						/* TODO ; treat as critical? */
						ALLM_ERROR((L"unable to equip one-piece there is a part of one-piece"));
						return false;
					}
				}

				// ������ ��û�� ť�� ���� - ���� bool���� �ʿ����� �ʴ�.
				if (!itemRequirement.RequireDirectlyEquip(itemHash, instance, serial, equipPos))
				{
					ALLM_ERROR((L"Items::RequireDirectlyEquip() - Error : RequireDirectlyEquip() == false"));
					return false;
				}

				// ������ ��û
				if (!event.ItemsEquip(bankItem, instance, equipPos))
				{
					ALLM_ERROR((L"Items::PutOnEquip() - Error : event.ItemsEquip() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::NoResponse;
					Send(error);

					return false;
				}

				return true;
			}

			bool Items::DropItems(uint32 bag, uint32 pos, uint32 count, bool sendPacket, Common::ItemEx* droppedItem)
			{
				bool result = false;

				/* TODO ; make unable to drop if it is cash item */

				// Equip Drop
				if (Database::DatabaseInstance().InfoCollections.BagInfos.EqualBagType(Database::Info::BagInfos::BagType::Equip, bag))
				{
					if (count != 1)
					{
						ALLM_INFO((L"Items::DropItems(Equip) - Error : if(count({}) != 1)", count));
						return false;
					}

					if (!itemRequirement.IsEmpty())
					{
						SenditemRequirementError();
						return false;
					}

					// ������ ��� ��û
					return RequireDropEquip(pos);
				}

				// �κ� ���� ������
				// for log
				const Common::ItemEx* pItem = inventory.GetItem(bag, pos);
				if (pItem == NULL)
				{
					ALLM_WARNING((L"Items::DropItems() inventory.GetItem({}-1, {}) == NULL", bag, pos));
					return false;
				}
				if ((count > 0) && (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(pItem->Info->Hash) == true)) {
					XRated::PlayerData& data = player->GetPlayerData();
					auto user = std::static_pointer_cast<User>(data.user);
					if (user == NULL) {
						ALLM_ERROR((L"Item::DropItems failed : pet item, user == NULL"));
						return false;
					}
					//GetPetData�� ����ϱ� ���ؼ��� GetSyncObject()�� �ɾ�� �Ѵ�.
					AutoLock cs(user->GetPetDatas().GetSyncObject());
					const XRated::Pet* pet = user->GetPetDatas().GetPetData(pItem->Serial);
					if (pet != NULL) {
						//���� ��ȯ �Ǿ� �ִ� ���¶�� ������ ������ ���д�.
						if (pet->Appear == true) {
							user->CriticalError(fmt::format("DropItem : Wrong Request, appear pet : {}", pItem->Serial).c_str(), false);
							return false;
						}
					}
				}

				{
					Common::ItemEx itemForLog = *pItem;
					itemForLog.StackedCount = count;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
					Log("dropitem", itemForLog);
				}

				result = inventory.DropItem(bag, pos, count, droppedItem);

				if (result)
				{
					if (sendPacket)
					{
						Protocol::FromServer::DropItem dropItem;
						dropItem.bag = static_cast<uint16>(bag);
						dropItem.pos = static_cast<uint16>(pos);
						dropItem.DroppedCount = static_cast<uint16>(count);
						dropItem.StackedCount = (pItem->Info ? pItem->StackedCount : 0);
						Send(dropItem);
					}
				}
				else
				{
					Common::ItemEx itemForLog = *pItem;
					itemForLog.StackedCount = count;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
					Log("dropitem-fail", itemForLog);
				}

				return result;
			}

			bool Items::DropItemsInPetInventory(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType posType, uint8 pos, uint32 count, bool sendPacket, Common::ItemEx* droppedItem)
			{
				bool result = false;

				// Equip Drop
				if (posType == XRated::PetItemSlot::Equipment)
				{
					if (count != 1)
					{
						ALLM_INFO((L"Items::DropItemsInPetInventory(Equip) - Error : if(count({}) != 1)", count));
						return false;
					}

					if (!itemRequirement.IsEmpty())
					{
						SenditemRequirementError();
						return false;
					}

					// ������ ��� ��û
					//return RequireDropEquip(pos);
				}

				// �κ� ���� ������
				// for log
				const Common::ItemEx* pItem = petsInventory.GetItem(petSerial, posType, pos);
				if (pItem == NULL || pItem->Info == NULL)
				{
					ALLM_WARNING((L"Items::DropItemsInPetInventory() inventory.GetItem({}, {}) == NULL", posType, pos));
					return false;
				}

				{
					Common::ItemEx itemForLog = *pItem;
					itemForLog.StackedCount = count;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
					Log("dropitem", itemForLog);
				}

				result = petsInventory.DropItem(petSerial, posType, pos, count);

				if (result)
				{
					if (sendPacket)
					{
						Protocol::FromServer::DropItemInPetInventory dropItem;

						dropItem.PetSerial = petSerial;
						dropItem.PositionType = posType;
						dropItem.Position = pos;
						dropItem.StackCount = pItem->StackedCount;

						Send(dropItem);
					}
				}
				else
				{
					Common::ItemEx itemForLog = *pItem;
					itemForLog.StackedCount = count;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
					Log("dropitem-fail", itemForLog);
				}

				return result;
			}
			bool Items::MoveItemInventoryToPet(const XRated::GlobalSerial& petSerial, uint8 inventoryBag, uint8 inventoryPosition, XRated::PetItemSlot::PositionType positionType, uint8 petInventoryPosition)
			{
				AutoLock lock(cs);

				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				if (inventoryBag == 0)
					return false;

				const Common::ItemEx* userItem = inventory.GetItem((inventoryBag), inventoryPosition);
				const Common::ItemEx* petItem = petsInventory.GetItem(petSerial, positionType, petInventoryPosition);

				if (petItem == NULL)
				{
					event.CriticalError(fmt::format("MoveItemInventoryToPet : petItem({}), petSerial : {}", petItem->Info->Hash, petSerial).c_str(), false, 0);
					return false;
				}

				Protocol::FromServer::MoveItemInventoryToPet petMoveItem;
				petMoveItem.Inventory = XRated::ItemPosition::Invalid;

				if (petItem->Info != NULL || userItem != NULL)
				{
					if (positionType == XRated::PetItemSlot::PositionType::Equipment)
					{
						bool isEquip = false;
						uint32 equipId = 0;
						const Database::Info::ItemInfo* equipItemInfo = NULL;
						int64 equipItemInstance = 0;
						uint32 invenId = 0;
						if (petItem->Info != NULL) {
							equipId = petItem->Info->Hash;
						}
						if (userItem != NULL) {
							isEquip = true;
							invenId = userItem->Info->Hash;
							//���� ������ �ִ� ��� ���� Ȯ��.
							if (userItem->Info->ItemType != Database::Info::ItemInfo::Type::PETEQUIPMENT) {
								return false;
							}
							if (userItem->Info->EquipParts != XRated::Constants::GetEquipParts(static_cast<XRated::Constants::Equipment>(petInventoryPosition))) {
								return false;
							}
							equipItemInfo = userItem->Info;
							equipItemInstance = userItem->InstanceEx;
						}


						if (itemRequirement.RequirePetMoveEquip(isEquip, petSerial, equipId, petInventoryPosition, invenId, inventoryPosition, (inventoryBag)) == false) {
							ALLM_ERROR((L"Items::MoveItemInventoryToPet() - Error : RequirePetMoveEquip() == false"));
							return false;
						}
						if (event.PetItemsEquip(petSerial, equipItemInfo, equipItemInstance, petInventoryPosition) == false) {
							ALLM_ERROR((L"Items::MoveItemInventoryToPet() - Error : event.PetItemsEquip() == false"));
							itemRequirement.Clear();

							Protocol::FromServer::Error error;
							error.errorcode = Errors::NoResponse;
							Send(error);

							return false;
						}
						return true;
					}
					else
					{
						Common::ItemEx userTempItem;
						Common::ItemEx petTempItem;

						if (userItem != NULL)
						{
							if (petItem->Info == NULL)
							{
								petTempItem.Info = NULL;
								petTempItem.InstanceEx = 0;
								petTempItem.Serial = 0;
								petTempItem.StackedCount = 0;
							}
							else
								petTempItem = *petItem;

							userTempItem = *userItem;
						}
						else
						{
							petTempItem = *petItem;

							userTempItem.Info = NULL;
							userTempItem.InstanceEx = 0;
							userTempItem.Serial = 0;
							userTempItem.StackedCount = 0;
						}
						if (inventory.SetItem(inventoryBag, inventoryPosition, petTempItem) == false)
							return false;

						if (petsInventory.SetItem(petSerial, positionType, petInventoryPosition, userTempItem) == false) {
							inventory.SetItem(inventoryBag, inventoryPosition, userTempItem);
							return false;
						}

						//�� �α�...
						if (userTempItem.Info != NULL) {
							/*Logger().Write(IAdmLogger::ImportantLogger, "petbag-in", event.GetName(), Service::Http::Logger::Parameter()
								<< userTempItem.Info->Hash << userTempItem.Serial << userTempItem.StackedCount << userTempItem.InstanceEx.Instance
								<< userTempItem.InstanceEx.ExpireDate.ToString() << petSerial);*/
						}
						if (petTempItem.Info != NULL) {
							/*Logger().Write(IAdmLogger::ImportantLogger, "petbag-out", event.GetName(), Service::Http::Logger::Parameter()
								<< petTempItem.Info->Hash << petTempItem.Serial << petTempItem.StackedCount << petTempItem.InstanceEx.Instance
								<< petTempItem.InstanceEx.ExpireDate.ToString() << petSerial);*/
						}
					}

					petMoveItem.PetSerial = petSerial;
					petMoveItem.Inventory.Bag = inventoryBag;
					petMoveItem.Inventory.Position = inventoryPosition;
					petMoveItem.PetSlotType = positionType;
					petMoveItem.PetSlotPosition = petInventoryPosition;

					Send(petMoveItem);
					return true;
				}

				return false;

				//TODO : �κ� <-> �� �κ� �̵�.
			}

			bool Items::MoveItemInPetInven(const XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType sourcePositionType, uint8 sourcePosition,
				XRated::PetItemSlot::PositionType targetPositionType, uint8 targetPosition)
			{
				AutoLock lock(cs);

				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				const Common::ItemEx* sourceItem = petsInventory.GetItem(petSerial, sourcePositionType, sourcePosition);
				const Common::ItemEx* targetItem = petsInventory.GetItem(petSerial, targetPositionType, targetPosition);

				if (sourceItem == NULL || targetItem == NULL)
				{
					event.CriticalError(fmt::format("MoveItemInPetInven : soureItem({}), targetItem({}), petSerial : {}", sourceItem->Info->Hash, targetItem->Info->Hash, petSerial).c_str(), false, 0);
					return false;
				}

				Common::ItemEx source = *sourceItem;
				Common::ItemEx target = *targetItem;

				if (sourcePositionType != XRated::PetItemSlot::Equipment && targetPositionType != XRated::PetItemSlot::Equipment)
				{
					////////////////// Need PetInvenValidation - �� ���â�� ����Ϸ��� �ϴ°� �������̼��ؾ���.
					if (petsInventory.SetItem(petSerial, sourcePositionType, sourcePosition, target) == false)
					{
						ALLM_ERROR((L"Items::MoveItemInPetInven() Error - SetItem(sourceSlot)"));
						return false;
					}
					if (petsInventory.SetItem(petSerial, targetPositionType, targetPosition, source) == false)
					{
						ALLM_ERROR((L"Items::MoveItemInPetInven() Error - SetItem(targetSlot)"));

						/// �ѹ�? �̰� �� ����� Yong-Eun, Jung 2008.06.10
						petsInventory.SetItem(petSerial, sourcePositionType, sourcePosition, source);
						return false;
					}

					Protocol::FromServer::MoveItemInPetInven sendPacket;

					sendPacket.PetSerial = petSerial;
					sendPacket.SourcePositionType = sourcePositionType;
					sendPacket.SourcePosition = sourcePosition;
					sendPacket.TargetPositionType = targetPositionType;
					sendPacket.TargetPosition = targetPosition;

					Send(sendPacket);
				}
				else
				{
					uint8 invenPos = sourcePosition;
					uint8 equipPos = targetPosition;
					bool isEquip = true;

					if (targetPositionType != XRated::PetItemSlot::Equipment)
					{
						if (target.Info == NULL)
							isEquip = false;

						invenPos = targetPosition;
						equipPos = sourcePosition;

						source = *targetItem;
						target = *sourceItem;

						XRated::PetItemSlot::PositionType tempType = sourcePositionType;
						sourcePositionType = targetPositionType;
						targetPositionType = tempType;
					}

					if (targetPositionType == XRated::PetItemSlot::Equipment)
					{
						if (isEquip == true)
						{
							//���� ������ �ִ� ��� ���� Ȯ��.
							if (source.Info->ItemType != Database::Info::ItemInfo::Type::PETEQUIPMENT)
								return false;
							if (source.Info->EquipParts != XRated::Constants::GetEquipParts(static_cast<XRated::Constants::Equipment>(equipPos)))
								return false;
						}

						if (itemRequirement.RequireMovePetEquipInPetInven(isEquip, petSerial, source.Info != NULL ? source.Info->Hash : 0, equipPos, target.Info != NULL ? target.Info->Hash : 0, invenPos) == false)
						{
							ALLM_ERROR((L"Items::MoveItemInPetInven() - Error : RequireMovePetEquipInPetInven() == false"));
							return false;
						}
						if (event.PetItemsEquip(petSerial, source.Info, source.InstanceEx, equipPos) == false)
						{
							ALLM_ERROR((L"Items::MoveItemInPetInven() - Error : event.PetItemsEquip() == false"));
							itemRequirement.Clear();

							Protocol::FromServer::Error error;
							error.errorcode = Errors::NoResponse;
							Send(error);

							return false;
						}
					}
				}

				return true;
			}

			bool Items::UseItemInPetInven(const XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType positionType, uint8 position)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				const Common::ItemEx* item(petsInventory.GetItem(petSerial, positionType, position));

				if (item == NULL || item->Info == NULL || !(item->Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USE))
				{
					uint32 itemHash = 0;
					if ((item != NULL) && (item->Info != NULL)) {
						itemHash = item->Info->Hash;
					}
					ALLM_ERROR((L"invalid item to use in pet invnetory, pos({}, {}) : item - {}",
						static_cast<int>(positionType), static_cast<int>(position), itemHash));
					return false;
				}

				/* use-item validation */
				if (player == NULL)
				{
					ALLM_ERROR((L"not expected"));
					return false;
				}

				if (IsAbleToUse(player, item->Info, event.GetStageInfo()) == false) // client should validate this
				{
					ALLM_ERROR((L"invalid item to use by limits"));
					return false;
				}

				// ������ ��û�� ť�� ����
				if (!itemRequirement.RequireUseItemInPetInven(petSerial, positionType, position))
				{
					ALLM_ERROR((L"Items::UseItemInPetInven() - Error : RequireUseItemInPetInven() == false"));
					return false;
				}

				// ������ ��û
				if (!event.ItemsUse(item->Info->Hash, positionType, position))
				{
					ALLM_ERROR((L"Items::UseItemInPetInven() - Error : event.ItemsUse() == false"));
					itemRequirement.Clear();

					Protocol::FromServer::Error error;
					error.errorcode = Errors::UnableToUseItem;
					Send(error);

					return false;
				}

				return true;
			}

			bool Items::TakeOutAllPetItems(const XRated::GlobalSerial petSerial)
			{
				AutoLock lock(cs);

				Protocol::FromServer::TakeOutAllPetItems packet;

				packet.PetSerial = petSerial;
				packet.Result = packet.Ok;

				if (petsInventory.IsItemInPetInventory(petSerial) == false)
				{
					packet.Result = packet.PetInventoryEmpty;
					Send(packet);

					return true;
				}

				if (GetEmptySlotCount() == 0)
				{
					packet.Result = packet.InventorySlotFull;
					Send(packet);

					return true;
				}

				std::vector<Common::PetItemSlotEx> allPetItems;
				petsInventory.GetAllItems(petSerial, allPetItems);

				size_t moveItemCount = GetEmptySlotCount() < static_cast<uint32>(allPetItems.size()) ? GetEmptySlotCount() : static_cast<uint32>(allPetItems.size());

				if (moveItemCount != allPetItems.size())
					packet.Result = packet.InventorySlotInsufficient;

				for (size_t i = 0; i < moveItemCount; ++i)
				{
					Common::ItemEx item;

					item.Info = allPetItems[i].ItemInfo;
					item.InstanceEx = allPetItems[i].InstanceEx;
					item.Serial = allPetItems[i].Serial;
					item.StackedCount = allPetItems[i].Stacked;

					if (ItemAcquired(allPetItems[i].ItemInfo->Hash, 0, allPetItems[i].Stacked, allPetItems[i].InstanceEx,
						true, XRated::Constants::AddItemType::NoneAddItemType, allPetItems[i].Serial) == false)
					{
						ALLM_ERROR((L"User::TakeOutAllPetItems() - failed : Add item to inventory, Pet({}) Inven Slot {} {}",
							allPetItems[i].PetSerial, static_cast<int>(allPetItems[i].Type), static_cast<int>(allPetItems[i].Position)));

						return false;
					}

					if (petsInventory.DropItem(allPetItems[i].PetSerial, allPetItems[i].Type, allPetItems[i].Position) == true)
					{
						/// DropItem Method �ȿ� �ְ�������.... ������ �������� �Ͼ�� ���ڵ�.. �� ���⼭ by Yong-Eun, Jung 2008.07.10
						Protocol::FromServer::DropItemInPetInventory dropItemInPetInventoryPacket;

						dropItemInPetInventoryPacket.PetSerial = allPetItems[i].PetSerial;
						dropItemInPetInventoryPacket.PositionType = allPetItems[i].Type;
						dropItemInPetInventoryPacket.Position = allPetItems[i].Position;
						dropItemInPetInventoryPacket.StackCount = 0;

						Send(dropItemInPetInventoryPacket);
					}
					/// ItemAcquired�� ó���ϴϱ� Position�� �� ���� ������ - Yong-Eun, Jung 2008-07-14
					//else
					//{
					//	DropItem(itemPosition);

					//	ALLM_ERROR((L"User::TakeOutAllPetItems() - Error :  Drop item to Pet Inventory , Pet({}) Inven Slot {} {}",
					//		allPetItems[i].PetSerial, static_cast<int>(allPetItems[i].Type), static_cast<int>(allPetItems[i].Position)));

					//	return false;
					//}

				}

				Send(packet);

				return true;
			}

			const Common::ItemEx* Items::GetPetItem(XRated::GlobalSerial petSerial, XRated::PetItemSlot::PositionType positionType, uint8 position)
			{
				return petsInventory.GetItem(petSerial, positionType, position);
			}
			uint16 Items::PetEquipPutOned(PetMoveEquipRequirement& result)
			{
				// �κ� â �˻�
				const Common::ItemEx* pInvenItem = inventory.GetItem(result.Equiped_IBag(), result.Equiped_IPos());
				if (pInvenItem == NULL)
				{
					ALLM_ERROR((L"User::PetEquipPutOned() - Error : pInvenItem == NULL"));
					itemRequirement.Clear();
					return Constants::InvalidPosition;
				}

				/* TODO : validation for empty slot count for one-piece equipment */
				Common::ItemEx invenItem = *pInvenItem; // �̰� �����ؾ� �Ѵ�.
				if (Database::Enchant::IsEnchantable(invenItem.Info)
					&& !(invenItem.Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEAFTERUSE)) // update HasEquipped state, check TRADEAFTERUSE attribute to keep compatibility for nerf
					reinterpret_cast<Database::Enchant::EnchantBitfields&>(invenItem.InstanceEx).HasEquipped = 1;
				else if (Database::Enchant::IsCashEnchantable(*invenItem.Info)
					&& !(invenItem.Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEAFTERUSE)) // update HasEquipped state, check TRADEAFTERUSE attribute to keep compatibility for nerf
					reinterpret_cast<Database::Enchant::NormalBitfields&>(invenItem.InstanceEx).HasEquipped = 1;

				// ���â �˻�
				const Common::ItemEx* petItem = petsInventory.GetItem(result.petSerial, XRated::PetItemSlot::Equipment, result.Equiped_EPos());

				// �ϴ� �κ����� ����
				inventory.DeleteItem(result.Equiped_IBag(), result.Equiped_IPos());


				//pet equip�� ���� ��� �־���.
				if (petItem->Info != NULL) {
					inventory.SetItem(result.Equiped_IBag(), result.Equiped_IPos(), *petItem);

					/*Logger().Write(IAdmLogger::ImportantLogger, "petbag-out", event.GetName(), Service::Http::Logger::Parameter()
						<< petItem->Info->Hash << petItem->Serial << petItem->StackedCount << petItem->InstanceEx.Instance
						<< petItem->InstanceEx.ExpireDate.ToString() << result.petSerial);*/
				}

				if (petsInventory.SetItem(result.petSerial, XRated::PetItemSlot::Equipment, result.Equiped_EPos(), invenItem) == false) {
					//petInventory���� �̵��� ���� �Ͽ��� ������ �ѹ��Ѵ�-.-?
					inventory.SetItem(result.Equiped_IBag(), result.Equiped_IPos(), invenItem);
					return Constants::InvalidPosition;
				}
				/*Logger().Write(IAdmLogger::ImportantLogger, "petbag-in", event.GetName(), Service::Http::Logger::Parameter()
					<< invenItem.Info->Hash << invenItem.Serial << invenItem.StackedCount << invenItem.InstanceEx.Instance
					<< invenItem.InstanceEx.ExpireDate.ToString() << result.petSerial);*/
				//�̵��� �������� ��� ���� ������.
				//petItemTemp�� �ִٸ� ���� ���� ����.
				//userItemTemp�� ������ ����.


				//petInventory�� �̵���Ų��.
				Protocol::FromServer::MoveItemInventoryToPet petMoveItem;
				petMoveItem.PetSerial = result.petSerial;
				petMoveItem.Inventory.Bag = static_cast<uint8>(result.Equiped_IBag());
				petMoveItem.Inventory.Position = static_cast<uint8>(result.Equiped_IPos());
				petMoveItem.PetSlotType = XRated::PetItemSlot::Equipment;
				petMoveItem.PetSlotPosition = static_cast<uint8>(result.Equiped_EPos());

				Send(petMoveItem);

				itemRequirement.Clear();
				return petMoveItem.PetSlotPosition;
			}
			uint16 Items::PetEquipTakeOffed(PetMoveEquipRequirement& result)
			{
				// �κ� â �˻�
				// ���â �˻�
				const Common::ItemEx* petItem = petsInventory.GetItem(result.petSerial, XRated::PetItemSlot::Equipment, result.Equiped_EPos());
				if (petItem == NULL)
				{
					ALLM_ERROR((L"User::EquipPutOffed() - Error : PetEquipItem == NULL"));
					itemRequirement.Clear();
					return Constants::InvalidPosition;
				}

				Common::ItemEx equipItem = *petItem; // �̰� �κ��� �־�� ��

				// �κ� â �˻�
				const Common::ItemEx* pInvenItem = inventory.GetItem(result.Equiped_IBag(), result.Equiped_IPos());

				// ���... ���� �� ����ֳ�
				if (pInvenItem != NULL)
				{
					ALLM_ERROR((L"User::EquipPutOffed() - pInvenItem({}) != NULL", pInvenItem->Info->Hash));
					itemRequirement.Clear();
					return Constants::InvalidPosition;
				}
				if (petsInventory.DropItem(result.petSerial, XRated::PetItemSlot::Equipment, result.Equiped_EPos()) == false) {
					ALLM_ERROR((L"User::EquipPutOffed() - PetEquip DropItem({},{}) failed", result.petSerial, result.Equiped_EPos()));
					return false;
				}
				inventory.SetItem(result.Equiped_IBag(), result.Equiped_IPos(), equipItem);

				/*Logger().Write(IAdmLogger::ImportantLogger, "petbag-out", event.GetName(), Service::Http::Logger::Parameter()
					<< equipItem.Info->Hash << equipItem.Serial << equipItem.StackedCount << equipItem.InstanceEx.Instance
					<< equipItem.InstanceEx.ExpireDate.ToString() << result.petSerial);*/

				//�̵��� �������� ��� ���� ������.
				//petItemTemp�� �ִٸ� ���� ���� ����.
				//userItemTemp�� ������ ����.		
				//petInventory�� �̵���Ų��.
				Protocol::FromServer::MoveItemInventoryToPet petMoveItem;
				petMoveItem.PetSerial = result.petSerial;
				petMoveItem.Inventory.Bag = static_cast<uint8>(result.Equiped_IBag());
				petMoveItem.Inventory.Position = static_cast<uint8>(result.Equiped_IPos());
				petMoveItem.PetSlotType = XRated::PetItemSlot::Equipment;
				petMoveItem.PetSlotPosition = static_cast<uint8>(result.Equiped_EPos());

				Send(petMoveItem);

				itemRequirement.Clear();
				return petMoveItem.PetSlotPosition;
			}
			void Items::DropItem(ItemPosition position)
			{
				if (position.Bag == 0) // equipment bag number
				{
					equip.DeleteItem(position.Position);
				}
				else
				{
					inventory.DeleteItem(position.Bag, position.Position);
				}
			}

			std::vector<const Database::Info::ItemInfo*> Items::GetPassiveItems() const
			{
				std::vector<const Database::Info::ItemInfo*> ret;
				struct Condition : Common::ItemEx::IFindCondition
				{
					bool operator()(const Common::ItemEx& rhs) const { return (rhs.Info->Attributes.Attr & (1 << 10)/*Database::Info::ItemInfo::ATTRIBUTES::PASSIVE*/) ? true : false; }
				};

				std::vector<const Common::ItemEx*> result(inventory.GetItems(Condition()));
				for (std::vector<const Common::ItemEx*>::const_iterator i = result.begin(); i != result.end(); ++i)
					ret.push_back((*i)->Info);

				return ret;
			}

			// arcrus. Fill inventory with designated item.
			uint32 Items::FillItem(uint32 hash, uint32 amount)
			{
				uint32 remaining;
				uint32 stackCount;
				remaining = stackCount = amount;

				const Database::Info::ItemInfo* itemInfo = GetItemInfo(hash);
				if (itemInfo == NULL)
				{
					ALLM_ERROR((L"Items::FillItem() - itemInfo({}) == NULL"
						, hash
						));

					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"GetItemInfo failed.";
					Send(error);
					return remaining;
				}

				if (amount == 0)
					return 0;
				do
				{
					if (inventory.GetEmptySlotCountByType(itemInfo) > 0)
					{
						if (remaining >= itemInfo->MaxCnt)
						{
							if (!ItemAcquired(hash, 0, itemInfo->MaxCnt, 0, true, XRated::Constants::AddItemType::NoneAddItemType, false))
							{
								ALLM_EXCEPTION((L"Items::FillItem() - unexpected ItemAcquired exception - remaining = {}, stackCount = {}"
									, remaining
									, stackCount
									));
								return remaining;
							}
							else
							{
								remaining -= itemInfo->MaxCnt;
								stackCount = remaining;
							}
						}
						else
						{
							if (!ItemAcquired(hash, 0, remaining, 0, true, XRated::Constants::AddItemType::NoneAddItemType, false))
							{
								ALLM_EXCEPTION((L"Items::FillItem() - unexpected ItemAcquired exception - remaining = {}, stackCount = {}"
									, remaining
									, stackCount
									));
								return remaining;
							}
							else
							{
								remaining = stackCount = 0;
							}
						}
					}
					else
					{
						if (!ItemAcquired(hash, 0, stackCount, 0, true, XRated::Constants::AddItemType::NoneAddItemType, false))
						{
							stackCount -= 1;
						}
						else
						{
							remaining -= stackCount;
							stackCount = remaining;
						}
					}
				} while (remaining > 0 && stackCount > 0);
				if (remaining > amount) {
					ALLM_EXCEPTION((L"Items::FillItem() - unexpected ItemAcquired exception - hash={} remaining={}, amount={}", hash, remaining, amount));
					remaining = 0;
				}
				return remaining;
			}


			bool Items::Buy(Protocol::ToServer::Buy& buy, bool /*isPcRoom*/, float discountRate)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				if (buy.itemid == 0 || buy.count == 0)
				{
					ALLM_ERROR((L"Items::Buy() - Id : {}, count : {}", buy.itemid, (int)buy.count));
					return false;
				}


				const Database::Info::ItemInfo* pItemInfo = GetItemInfo(buy.itemid);
				const Database::Info::ItemInfo* requestBuyItem = pItemInfo;

				if (pItemInfo == NULL || pItemInfo->ShopType == XRated::Constants::ShopType::Decorative /*??*/)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughMoney;
					error.errorstring = L"������ ������ �����ϴ�.";
					Send(error);
					return false;
				}

				if (pItemInfo->Price < pItemInfo->SellPrice)
				{
					ALLM_ERROR((L"Items::Buy() - BuyPrice < SellPrice"));
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					Send(error);
					return false;
				}

				unsigned int totalPrice = static_cast<unsigned int>((float)pItemInfo->Price * buy.count * discountRate + 0.5);

				//if (isPcRoom) totalPrice = static_cast<unsigned int>(totalPrice * 0.8f);

				if (totalPrice > player->GetPlayerData().BaseCharacter.Money)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughMoney;
					Send(error);
					return false;
				}

				if (pItemInfo->LadderPoint != 0 && pItemInfo->LadderPoint > player->GetLadderPoint())  //asdf ȣ���� LAdderPoint ������
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughLadderPoint;
					Send(error);
					return false;
				}

				Protocol::FromServer::Buy sendbuy;
				sendbuy.itemid = buy.itemid;
				sendbuy.count = buy.count;
				sendbuy.instanceEx = 0;

				bool identified = false;
				if ((pItemInfo->ItemType == Database::Info::ItemInfo::Type::UNIDENTIFIED) && (buy.count == 1))
				{
					const XRated::UnidentifiedItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem(pItemInfo->Hash);
					if (info == NULL) {
						ALLM_EXCEPTION((L"BuyItem : UNIDENTIFIED item can not found UnidentifiedItemInfo : {}", pItemInfo->Hash));
						Protocol::FromServer::Error error;
						error.errorcode = Errors::Unexpected;
						Send(error);
						return false;
					}
					if (info->AutomaticIdentify) {
						ItemBasicInfo result;
						if (GetIdentifiedItem(pItemInfo->Hash, result) == true) {
							sendbuy.itemid = result.ItemHash;
							sendbuy.count = result.StackCount;
							sendbuy.instanceEx = result.InstanceEx;
							pItemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve(result.ItemHash);
							identified = true;
						}
					}
				}

				ItemPosition pos(AddItem(pItemInfo, sendbuy.count, sendbuy.instanceEx));
				if (pos == ItemPosition::Invalid)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::InventoryFull;
					error.errorstring = Errors::ToString(Errors::InventoryFull);
					Send(error);
					return false;
				}

				player->GetPlayerData().BaseCharacter.Money -= totalPrice;

				sendbuy.bag = static_cast<uint16>(pos.Bag); // you don't need to -1 bag because it is calculated in AddItem method
				sendbuy.pos = static_cast<uint16>(pos.Position);
				sendbuy.money = player->GetPlayerData().BaseCharacter.Money;
				Send(sendbuy);

				const Common::ItemEx& buyItem = *GetItem(pos.Bag, pos.Position);
				GlobalSerial	itemSerial = buyItem.Serial;
				InstanceEx itemInstance(buyItem.InstanceEx);
				if (identified == true) {
					//���� �������� Ǫ�� ��쿡�� itemSerial�� instance�� �߱� �Ǳ� ���� Ǯ�� ������ 0���� �ִ´�.
					itemSerial = 0;
					itemInstance = 0;
				}

				//web log.
				if (IsImportantToLog(requestBuyItem)) {
					int percent = static_cast<int>(discountRate * 100);
					std::wstring discount;
					if (percent != 100) {
						discount = StringUtil::To<std::wstring>(percent) + L"%";
					}
					/*Logger().Write(IAdmLogger::ImportantLogger, "buyitem", event.GetName(), Service::Http::Logger::Parameter()
						<< buy.itemid << itemSerial << buy.count << itemInstance.Instance << itemInstance.ExpireDate.ToString() << discount.c_str());*/
				}
				if (identified == true) {
					/* log the unidentified box */
					//Logger().Write(IAdmLogger::ImportantLogger, "Identifyitem", event.GetName(), Service::Http::Logger::Parameter()
					//	<< buy.itemid << itemSerial << buy.count << itemInstance.Instance << itemInstance.ExpireDate.ToString() << 0/*means a box*/);

					//���� �������� Ǯ���� ���õ� serial�� instance�� ������ش�.
					//GlobalSerial	itemSerial = buyItem.Serial;
					//InstanceEx itemInstance(buyItem.InstanceEx.Instance, buyItem.InstanceEx.ExpireDate);
					//Logger().Write(IAdmLogger::ImportantLogger, "Identifyitem", event.GetName(), Service::Http::Logger::Parameter()
					//	<< pItemInfo->Hash << itemSerial << sendbuy.count << itemInstance.Instance << itemInstance.ExpireDate.ToString() << 1/*means a result*/);
				}
				return true;
			}

			bool Items::BuyBack(Protocol::ToServer::BuyBack& buy, bool /*isPcRoom*/, float discountRate) // 3.1 BY ULTIMATE
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				if (buy.itemid == 0 || buy.count == 0)
				{
					ALLM_ERROR((L"Items::Buy() - Id : {}, count : {}", buy.itemid, (int)buy.count));
					return false;
				}

				const Database::Info::ItemInfo* pItemInfo = GetItemInfo(buy.itemid);

				// when we are buying back we can't use this condition "pItemInfo->ShopType==XRated::Constants::ShopType::Decorative"
				// because we will be able to rebuy any item
				//if( pItemInfo == NULL || pItemInfo->ShopType==XRated::Constants::ShopType::Decorative /*??*/ )
				if (pItemInfo == NULL)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughMoney;
					error.errorstring = L"Nulo?";
					Send(error);
					return false;
				}
				/*
						if ( pItemInfo->SellPrice < pItemInfo->SellPrice )
						{
							ALLM_ERROR(( L"Items::Buy() - BuyPrice < SellPrice" ));
							Protocol::FromServer::Error error;
							error.errorcode   = Errors::Unexpected;
							Send( error );
							return false;
						}
				*/
				unsigned int totalPrice = static_cast<unsigned int>((float)pItemInfo->SellPrice * buy.count);

				if (totalPrice > player->GetPlayerData().BaseCharacter.Money)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughMoney;
					Send(error);
					return false;
				}

				Protocol::FromServer::BuyBack sendbuy;
				sendbuy.itemid = buy.itemid;
				sendbuy.count = buy.count;
				sendbuy.instanceEx = 0;
				/*
						bool identified = false;
						if ( (pItemInfo->ItemType == Database::Info::ItemInfo::Type::UNIDENTIFIED) && (buy.count == 1))
						{
							const XRated::UnidentifiedItemInfo* info=Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem(pItemInfo->Hash);
							if( info == NULL ) {
								ALLM_EXCEPTION((L"BuyItem : UNIDENTIFIED item can not found UnidentifiedItemInfo : {}",pItemInfo->Hash));
								Protocol::FromServer::Error error;
								error.errorcode   = Errors::Unexpected;
								Send( error );
								return false;
							}
							if (info->AutomaticIdentify) {
								ItemBasicInfo result;
								if( GetIdentifiedItem(pItemInfo->Hash, result)  == true ) {
									sendbuy.itemid = result.ItemHash;
									sendbuy.count = result.StackCount;
									sendbuy.instanceEx = result.instanceEx;
									pItemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve( result.ItemHash );
									identified = true;
								}
							}
						}
				*/

				ItemPosition pos(AddItem(pItemInfo, sendbuy.count, sendbuy.instanceEx));
				if (pos == ItemPosition::Invalid)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::InventoryFull;
					error.errorstring = Errors::ToString(Errors::InventoryFull);
					Send(error);
					return false;
				}

				player->GetPlayerData().BaseCharacter.Money -= totalPrice;

				sendbuy.bag = static_cast<uint16>(pos.Bag); // you don't need to -1 bag because it is calculated in AddItem method
				sendbuy.pos = static_cast<uint16>(pos.Position);
				sendbuy.money = player->GetPlayerData().BaseCharacter.Money;
				Send(sendbuy);

				//HOTFIX FOR ACHIEVEMENT CHEATING
				/*
				if (event.IsAchievementConnected()) {
					// send gold type
					Protocol::Achievement::ServerProtocol::GoldGainChanged packet;
					packet.characterName = event.GetName();
					packet.checkAchieve = uint8(0);
					packet.goldGainType = Net::Protocol::Achievement::ServerProtocol::GoldGainChanged::goldGainType::Sell_Item;
					packet.gold = uint64(totalPrice) * -1;
					event.ItemsSendAchievement(packet);
					// check if has achieved
					packet.characterName = event.GetName();
					packet.checkAchieve = uint8(1);
					packet.goldGainType = Net::Protocol::Achievement::ServerProtocol::GoldGainChanged::goldGainType::Sell_Item;
					packet.gold = uint64(0);
					event.ItemsSendAchievement(packet);
				}
				*/
				//END HOTFIX FOR ACHIEVEMENT CHEATING

				const Common::ItemEx& buyItem = *GetItem(pos.Bag, pos.Position);
				/*
				GlobalSerial	itemSerial = buyItem.Serial;
				int64			itemInstance = buyItem.Instance;
				if( identified == true ) {
					itemSerial = 0;
					itemInstance = 0;
				}
				*/

				return true;
			}

			bool Items::BuyBarterItem(Protocol::ToServer::BuyBarterItem& buy, bool /* isPcRoom*/) // 3.1 BY ULTIMATE
			{
				int barterId = buy.barterItemId/*-1*/;

				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				if (buy.count == 0)
				{
					ALLM_ERROR((L"Items::BuyBarterItem() - Id : {}, count : {}", barterId, (int)buy.count));
					return false;
				}

				const Database::Info::BarterItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.BarterItems.Retrieve(barterId); // puxei os dados do .b
				const Database::Info::ItemInfo* pItemInfo = GetItemInfo(itemInfo->BuyItemId); // recebi parametros do ittem

				XRated::PlayerData& data = player->GetPlayerData();
				auto user = std::static_pointer_cast<User>(data.user);

				if (user == NULL) {
					ALLM_ERROR((L"Item::DropItems failed"));
					return false;
				}
				uint32 MyItemCount = user->GetItemCount(itemInfo->NeedItemId);

				if (MyItemCount < itemInfo->NeedItemCount) {
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughNeedItemCount_BuyBarter;
					error.errorstring = L"Nulo?";
					Send(error);
					return false;
				}

				if (pItemInfo->Price < pItemInfo->SellPrice)
				{
					ALLM_ERROR((L"Items::Buy() - BuyPrice < SellPrice"));
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					Send(error);
					return false;
				}

				if (itemInfo->BuyItemPrice > player->GetPlayerData().BaseCharacter.Money)
				{
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughMoney;
					Send(error);
					return false;
				}

				if (user->RemoveItems(itemInfo->NeedItemId, itemInfo->NeedItemCount) == true)
				{
					Protocol::FromServer::BuyBarterItem barterBuy;
					barterBuy.barterItemId = barterId;
					barterBuy.count = buy.count;
					Send(barterBuy);

					Protocol::FromServer::Buy sendbuy;
					sendbuy.itemid = itemInfo->BuyItemId;
					sendbuy.count = buy.count;
					sendbuy.instanceEx = 0;

					ItemPosition pos(AddItem(pItemInfo, sendbuy.count, sendbuy.instanceEx));
					if (pos == ItemPosition::Invalid)
					{
						Protocol::FromServer::Error error;
						error.errorcode = Errors::InventoryFull;
						error.errorstring = Errors::ToString(Errors::InventoryFull);
						Send(error);
						return false;
					}
					player->GetPlayerData().BaseCharacter.Money -= itemInfo->BuyItemPrice;

					sendbuy.bag = static_cast<uint16>(pos.Bag); // you don't need to -1 bag because it is calculated in AddItem method
					sendbuy.pos = static_cast<uint16>(pos.Position);
					sendbuy.money = player->GetPlayerData().BaseCharacter.Money;
					Send(sendbuy);

					const Common::ItemEx& buyItem = *GetItem(pos.Bag, pos.Position);

				}
				else {
					Protocol::FromServer::Error error;
					error.errorcode = Errors::NotEnoughNeedItemCount_BuyBarter;
					Send(error);
					return false;
				}
				return true;
			}

			bool Items::Sell(Protocol::ToServer::Sell& sell, bool /*isPcRoom*/)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				if (player == NULL)
				{
					ALLM_ERROR((L"Items::Sell() - player == NULL"));
					return false;
				}

				int bag = sell.bag;
				int pos = sell.pos;

				const Common::ItemEx* item = inventory.GetItem(bag, pos);
				if (item == NULL)
				{
					ALLM_ERROR((L"Items::Sell() - item == NULL"));
					return false;
				}

				if (item->Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::CASH) // temporary; ATTRIBUTES::CASH inidicates untradable at shop
				{
					ALLM_ERROR((L"Items::Sell() - pItemInfo == NULL or CASH"));
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"������ ������ �����ϴ�.";
					Send(error);
					return false;
				}

				//���Ű��ݺ��� �ǸŰ����� ������ ���� ���ϰ� �Ѵ�.
				if (item->Info->Price < item->Info->SellPrice)
				{
					ALLM_ERROR((L"Items::Sell() - BuyPrice < SellPrice"));
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					Send(error);
					return false;
				}

				Common::ItemEx tempItem(*item); // copy the item information to log

				//Gold Overflow Check. �̰� �ɸ��� Ŭ�󿡼� validate �������� �ִٴ°Ŵ�
				if ((player->GetPlayerData().BaseCharacter.Money) > (player->GetPlayerData().BaseCharacter.Money + tempItem.Info->SellPrice * tempItem.StackedCount))
				{
					ALLM_ERROR((L"Items::Sell() - Gold Overflow"));
					Protocol::FromServer::Error error;
					error.errorcode = Errors::GoldOverflow;
					error.errorstring = L"";
					Send(error);
					return false;
				}

				inventory.DeleteItem(bag, pos);

				unsigned int totalPrice = tempItem.Info->SellPrice * tempItem.StackedCount;
				player->GetPlayerData().BaseCharacter.Money += totalPrice;
				/*
				if (event.IsAchievementConnected()) {
					// send gold type
					Net::Protocol::Achievement::ServerProtocol::GoldGainChanged packet;
					packet.characterName = event.GetName();
					packet.checkAchieve = uint8(0);
					packet.goldGainType = Net::Protocol::Achievement::ServerProtocol::GoldGainChanged::goldGainType::Sell_Item;
					packet.gold = uint64(totalPrice);
					event.ItemsSendAchievement(packet);
					// check if has achieved
					packet.characterName = event.GetName();
					packet.checkAchieve = uint8(1);
					packet.goldGainType = Net::Protocol::Achievement::ServerProtocol::GoldGainChanged::goldGainType::Sell_Item;
					packet.gold = uint64(0);
					event.ItemsSendAchievement(packet);
				}

				*/
				Log("sellitem", tempItem);

				uint32 chipItemCredit = 0;

				if (tempItem.Info->Hash == XRated::Database::DatabaseInstance().InfoCollections.LuniaChipItems.GetChipHash(XRated::Gamble::ChipType::Pink))
					chipItemCredit = tempItem.StackedCount;
				else if (tempItem.Info->Hash == XRated::Database::DatabaseInstance().InfoCollections.LuniaChipItems.GetChipHash(XRated::Gamble::ChipType::Green))
					chipItemCredit = tempItem.StackedCount * 100;
				else if (tempItem.Info->Hash == XRated::Database::DatabaseInstance().InfoCollections.LuniaChipItems.GetChipHash(XRated::Gamble::ChipType::Yellow))
					chipItemCredit = tempItem.StackedCount * 10000;

				if (chipItemCredit > 0) // at least 1 chip sold.
				{
					// Logging : race-sellchip
					//Logger().Write(IAdmLogger::ImportantLogger, "race-sellchip", player->GetName(), Http::Logger::Parameter()
					//	<< chipItemCredit
					//	//<< tempItem.Info->SellPrice * tempItem.StackedCount
					//	<< totalPrice
					//);
				}

				return true;
			}

			bool Items::DropBankItems(uint32 bag, uint32 pos)
			{
				bank.DeleteItem(bag, pos);
				return true;
			}

			void Items::GetClassSpecializedItemTable(const XRated::UnidentifiedItemInfo::ProbabilityTableList& originTable, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions)
			{
				portions.clear();
				probabilityTable.clear();

				XRated::Constants::ClassType classType;

				{
					AutoLock lock(cs);
					if (player == NULL)
					{
						ALLM_EXCEPTION((L"[Items::GetClassSpecializedItemTable] Unable to find player information."));
						return;
					}
					classType = player->GetClassType();
				}
				for (std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>::const_iterator i = originTable.begin()
					; i != originTable.end(); ++i)
				{
					const Database::Info::ItemInfo* itemInfo(GetItemInfo(i->Id.c_str()));
					if (itemInfo == NULL)
					{
						ALLM_ERROR((L"unable to find item information of id [{}]", i->Id.c_str()));
						continue;
					}

					if ((itemInfo->Limits.Class == classType))
					{
						portions.push_back(i->Probability);
						probabilityTable.push_back(*i);
					}
				}

			}

			void Items::GetMajorSpecializedItemTable(const XRated::UnidentifiedItemInfo::ProbabilityTableList& originTable, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions)
			{
				portions.clear();
				probabilityTable.clear();

				/* Teles 3.1 MajorType Boxes*/
				XRated::Constants::CharacterMajorStatType majorType;

				{
					AutoLock lock(cs);
					if (player == NULL)
					{
						ALLM_EXCEPTION((L"[Items::GetClassSpecializedItemTable] Unable to find player information."));
						return;
					}
					majorType = player->GetMajorStatusType();
				}
				//printf("majorType[{}]\n", majorType);
				for (std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>::const_iterator i = originTable.begin()
					; i != originTable.end(); ++i)
				{
					const Database::Info::ItemInfo* itemInfo(GetItemInfo(i->Id.c_str()));
					if (itemInfo == NULL)
					{
						ALLM_ERROR((L"unable to find item information of id [{}]", i->Id.c_str()));
						continue;
					}
					//std::wstring itemName(itemInfo->Id.c_str());
					//std::wcout << "		[" << itemName << "][itemInfo->Limits.MajorStat] " << std::dec << itemInfo->Limits.MajorStat << std::endl;;
					if ((itemInfo->Limits.MajorStat == (int)majorType))
					{
						portions.push_back(i->Probability);
						probabilityTable.push_back(*i);
					}
				}
			}

			void Items::GetTotalProbabilityTable(const XRated::UnidentifiedItemInfo::ProbabilityTableList& originTable, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions)
			{
				portions.clear();
				probabilityTable.clear();

				for (std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>::const_iterator i = originTable.begin(); i != originTable.end(); ++i) {
					portions.push_back(i->Probability);
					probabilityTable.push_back(*i);
				}
			}

			void Items::GetProbabilityTable(XRated::UnidentifiedItemInfo* uInfo, std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable>& probabilityTable, std::vector<float>& portions)
			{
				if (uInfo != NULL) {

					uint16 characterLevel = 1;
					{
						AutoLock lock(cs);
						Logic::Player* temp = player;
						if (temp == NULL)
						{
							ALLM_EXCEPTION((L"[Items::GetClassSpecializedItemTable] Unable to find player information."));
							return;
						}
						characterLevel = temp->GetLevel();
					}
					const XRated::UnidentifiedItemInfo::ProbabilityTableList* list = uInfo->GetProbabilityTable(characterLevel);
					if (list != NULL) {
						if (Math::RandomByRate(uInfo->ClassSpecializedRate)) {
							GetClassSpecializedItemTable(*list, probabilityTable, portions);
						}
						else {
							GetMajorSpecializedItemTable(*list, probabilityTable, portions);
							//printf("GetMajor Called\n");
						}
						if (portions.empty() || probabilityTable.empty())
							GetTotalProbabilityTable(*list, probabilityTable, portions);
					}
				}
			}

			bool Items::MissionClearItemIdentify(const Database::Info::ItemInfo* item, XRated::RewardItem& rewardItem)
			{
				rewardItem.ItemHash = 0;
				rewardItem.StackCount = 0;

				if (!item)
				{
					ALLM_ERROR((L"Items::MissionClearItemIdentify() - itemInfo* == NULL"));
					return false;
				}

				rewardItem.ItemHash = item->Hash;
				rewardItem.StackCount = 1;

				/**
					TODO : MissionClear�ÿ��� �Ҹ��� ����� �Ʒ��� 2���� validation�� �̹� �ϰ� �Լ��� �Ѿ�´�.. ���� �� �ʿ� ����.
						   ������ �ٸ� ������ �� �Լ��� ���� �ȴٸ� �߰��� �ؾ��� ���̴�.
				*/
				//if( item->ItemType != Database::Info::ItemInfo::Type::UNIDENTIFIED )
				//{
				//	ALLM_ERROR(( L"Items::MissionClearItemIdentify() - ItemType != UNIDENTIFIED" ));
				//	return false;
				//}

				XRated::UnidentifiedItemInfo* uInfo = Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem(item->Hash);
				if (uInfo == NULL)
				{
					ALLM_ERROR((L"Items::MissionClearItemIdentify() - ItemInfo* == NULL"));
					return false;
				}

				///* recursive auto-identify */
				//if ( !uInfo->AutomaticIdentify )
				//{
				//	ALLM_ERROR(( L"Items::MissionClearItemIdentify Not AutomaticIdentify Item" ));
				//	return false;
				//}

				/* identify the item */
				std::vector<float> portions;
				std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable> probabilityTable;
				GetProbabilityTable(uInfo, probabilityTable, portions);

				if (portions.empty())
				{
					ALLM_ERROR((L"unable to find probability item"));
					return false;
				}

				Math::RandomByPortion r(portions.begin(), portions.end());
				const XRated::UnidentifiedItemInfo::ProbabilityTable& result(probabilityTable[r.Next()]);

				const Database::Info::ItemInfo* itemInfo(GetItemInfo(result.Id.c_str()));
				uint16 stackCount(result.StackedCount);

				// unable to find the result item info
				if (itemInfo == NULL)
				{
					ALLM_ERROR((L"Items::MissionClearItemIdentify() - unable to find item information of id [{}]", result.Id.c_str()));
					return false;
				}

				/* calculate instance fields */
				InstanceEx instance(0);

				if (Database::Enchant::IsEnchantable(itemInfo))
				{
					instance = result.InstanceEx;

					if (instance == InstanceEx(0)) {
						Database::Enchant::EnchantBitfields field = 0;
						if (uInfo->EnchantProbabilitys.ReinforcementProbabilitys.empty() == false) {
							Math::RandomByPortion r(uInfo->EnchantProbabilitys.ReinforcementProbabilitys.begin(), uInfo->EnchantProbabilitys.ReinforcementProbabilitys.end());
							size_t level = r.Next();
							if (level <= XRated::Constants::LocaleSpecificSetting::REINFORCEMENT_LEVEL) {
								field.Reinforcement = level;
							}
						}
						if (uInfo->EnchantProbabilitys.LightReinforcementProbabilitys.empty() == false) {
							Math::RandomByPortion r(uInfo->EnchantProbabilitys.LightReinforcementProbabilitys.begin(), uInfo->EnchantProbabilitys.LightReinforcementProbabilitys.end());
							size_t level = r.Next();
							if (level <= XRated::Constants::LocaleSpecificSetting::LIGHTREINFORCEMENT_LEVEL) {
								field.LightLevel = level;
							}
						}
						instance = static_cast<int64>(field);
					}
				}
				else if (itemInfo->ItemType == Database::Info::ItemInfo::Type::INSTANT) {
					ProcessInstantItem(itemInfo, stackCount);
					rewardItem.ItemHash = itemInfo->Hash;
					rewardItem.StackCount = stackCount;
					rewardItem.InstanceEx = instance;
					return true;
				}
				else if (itemInfo->MaxCnt == 1) // unenchantable
				{
					Database::Enchant::NormalBitfields field(0);

					DateTime expireDate(DateTime::Infinite);
					if (result.ExpireMin)
					{
						expireDate = DateTime::Now();
						int minSum = expireDate.Get(DateTime::Unit::Minute) + result.ExpireMin;
						if (minSum / DateTime::Constant::MinutesInAnHour != 0)
							expireDate.Add(DateTime::Unit::Hour, minSum / DateTime::Constant::MinutesInAnHour);
						expireDate.Add(DateTime::Unit::Minute, result.ExpireMin);
					}
					else if (result.ExpireDue)
					{
						expireDate = DateTime::Now();
						DateTime::Date temp = expireDate.GetDate();
						temp = temp.Add(DateTime::Unit::Month, result.ExpireDue + 1);
						temp.SetDay(1);

						expireDate = DateTime(temp, DateTime::Time());
					}
					else if (!result.ExpireDate.empty())
					{
						expireDate = DateTime(result.ExpireDate.c_str());
					}

					if (expireDate != DateTime::Infinite)
					{
						if (expireDate < DateTime::Now())
						{
							event.SendSystemChat(L"$UI.Global.SystemChat.IdentifiedItemIsExpired$");
							return false;
						}
						else
							instance.ExpireDate = expireDate;
					}

					field.CashEnchant1 = result.CashEnchant1Index;
					field.CashEnchant2 = result.CashEnchant2Index;
					field.CashEnchant3 = result.CashEnchant3Index;
					field.CashEnchant4 = result.CashEnchant4Index;

					instance = field;
				}

				rewardItem.ItemHash = itemInfo->Hash;
				rewardItem.StackCount = stackCount;
				rewardItem.InstanceEx = instance;

				//return true;
				return ItemAcquired(itemInfo->Hash, 0 /*Not Use*/, stackCount, instance);
			}

			void Items::ProcessInstantItem(const Database::Info::ItemInfo* itemInfo, uint16 stackCount)
			{
				for (std::vector<Database::Info::ActionInfo::State>::const_iterator i = itemInfo->States.begin(); i != itemInfo->States.end(); ++i) {
					switch (i->type) {
					case Database::Info::StateInfo::GOLDGAIN:
					{
						std::map<std::wstring, std::wstring>::const_iterator iter = i->params.find(L"Amount");
						AutoLock lock(cs); //player pointer�� null�� �Ǿ������ ������ lock�� �ɾ ó���ؾ� �Ѵ�.
						if (iter != i->params.end() && player != NULL) {
							uint32 owned = player->GetPlayerData().BaseCharacter.Money;
							int32 acquired = StringUtil::To<int32>(iter->second.c_str());
							acquired = acquired < 0 ? 0 : acquired;
							acquired *= stackCount;
							if ((owned > owned + (uint32)acquired)
								|| (owned + (uint32)acquired > XRated::Constants::GoldMaxLimit))
							{
								Protocol::FromServer::Error goldOverFlow;
								goldOverFlow.errorcode = Errors::GoldOverflow;
								goldOverFlow.errorstring = L"InstantItem";
								Send(goldOverFlow);
							}
							else
							{
								player->GetPlayerData().BaseCharacter.Money += (uint32)acquired;
								Protocol::FromServer::GoldGain packet;
								packet.type = XRated::Constants::GoldRewardAcquiredType::InstantCoin;
								packet.serial = player->GetSerial();
								packet.money = (uint32)acquired;

								Send(packet);
							}
						}
					}
					break;
					}
				}
			}

			bool Items::Identify(uint32 bag, uint32 pos, XRated::Item* acquiredItem/*Out*/)
			{
				if (!itemRequirement.IsEmpty())
				{
					SenditemRequirementError();
					return false;
				}

				/* inventory item validation */
				const Common::ItemEx* item = inventory.GetItem(bag, pos);
				if (item == NULL)
				{
					ALLM_ERROR((L"Items::Identify() - item == NULL"));
					return false;
				}
				if (item->Info->ItemType != Database::Info::ItemInfo::Type::UNIDENTIFIED)
				{
					ALLM_ERROR((L"Items::Identify() - ItemType != UNIDENTIFIED"));
					return false;
				}
				XRated::UnidentifiedItemInfo* uInfo = Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem(item->Info->Hash);
				if (uInfo == NULL)
				{
					ALLM_ERROR((L"Items::Identify() - UnidentifiedItemInfo* == NULL ({})", item->Info->Hash));
					return false;
				}

				/* log the unidentified box */
				//Logger().Write(IAdmLogger::ImportantLogger, "Identifyitem", event.GetName(), Service::Http::Logger::Parameter()
				//	<< item->Info->Hash << item->Serial << item->StackedCount << item->InstanceEx.Instance << item->InstanceEx.ExpireDate.ToString() << 0/*means a box*/);

				/* identify the item */
				std::vector<float> portions;
				std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable> probabilityTable;
				GetProbabilityTable(uInfo, probabilityTable, portions);

				if (portions.empty())
				{
					ALLM_ERROR((L"unable to find probability item"));
					return false;
				}

				Math::RandomByPortion r(portions.begin(), portions.end());
				const XRated::UnidentifiedItemInfo::ProbabilityTable& result(probabilityTable[r.Next()]);

				const Database::Info::ItemInfo* itemInfo(GetItemInfo(result.Id.c_str()));
				uint16 stackCount(result.StackedCount);

				// unable to find the result item info
				if (!itemInfo)
				{
					ALLM_ERROR((L"unable to find item information of id [{}]", result.Id.c_str()));
					return false;
				}

				/* calculate instance fields */
				InstanceEx InstanceEx(0);

				if (Database::Enchant::IsEnchantable(itemInfo))
				{
					InstanceEx = result.InstanceEx;
					if (InstanceEx == XRated::InstanceEx(0))
					{
						//강화.
						Database::Enchant::EnchantBitfields field = 0;
						if (uInfo->EnchantProbabilitys.ReinforcementProbabilitys.empty() == false)
						{
							Math::RandomByPortion r(uInfo->EnchantProbabilitys.ReinforcementProbabilitys.begin(), uInfo->EnchantProbabilitys.ReinforcementProbabilitys.end());
							size_t level = r.Next();
							if (level <= XRated::Constants::LocaleSpecificSetting::REINFORCEMENT_LEVEL)
							{
								field.Reinforcement = level;
							}
						}
						//빛 강화.
						if (uInfo->EnchantProbabilitys.LightReinforcementProbabilitys.empty() == false)
						{
							Math::RandomByPortion r(uInfo->EnchantProbabilitys.LightReinforcementProbabilitys.begin(), uInfo->EnchantProbabilitys.LightReinforcementProbabilitys.end());
							size_t level = r.Next();
							if (level <= XRated::Constants::LocaleSpecificSetting::LIGHTREINFORCEMENT_LEVEL)
							{
								field.LightLevel = level;
							}
						}
						InstanceEx = static_cast<int64>(field);
					}
				}
				else if (itemInfo->ItemType == Database::Info::ItemInfo::Type::INSTANT)
				{
					// Daru : Instant 속성을 가진 아이템이 나왔다면 그에 따른 처리를 해주자.
					// 정확한 instant 아이템의 처리는 logic으로 처리를 넘겨서 처리되어야 한다. 이 과정에 문제가 한가지 있는데,
					// Logic의처리는 비동기적으로 queue에 들어가게되고, 처리가 이루어지기 전에 유저의 접속이 끊어질 경우 롤백을 해줘야 한다.
					// 아이템에 대한 롤백은 현 시스템에서 지원되지 않는다. 정확한 처리는 유저의 접속 종료 처리시 모든 피드백이 Db나 로직에서 돌아왔는지
					// 검사해준 뒤 종료 처리를 해야 할 것 같다.
					ProcessInstantItem(itemInfo, stackCount);


				}
				else /*if (itemInfo->MaxCnt==1)*/ // unenchantable .. 기존에는 MaxCnt가 1인 아이템만 기간제를 줄 수 있었으나, 변경하자! 키득
				{
					Database::Enchant::NormalBitfields field(0);

					DateTime expireDate(DateTime::Infinite);
					if (result.ExpireMin)
					{
						expireDate = DateTime::Now();
						// dingpong. 남은 기한 처리 루틴.. minute 는 hour 과 다르게 datatime 내에서 day 쪽 추가 루틴이 구현이 안되어있기 때문에 hour 로 넘겨줘야한다.
						int minSum = expireDate.Get(DateTime::Unit::Minute) + result.ExpireMin;
						if (minSum / DateTime::Constant::MinutesInAnHour != 0)
							expireDate.Add(DateTime::Unit::Hour, minSum / DateTime::Constant::MinutesInAnHour);
						expireDate.Set(DateTime::Unit::Minute, minSum % DateTime::Constant::MinutesInAnHour);
					}
					else if (result.ExpireDue)
					{
						expireDate = DateTime::Now();
						DateTime::Date temp = expireDate.GetDate();
						temp = temp.Add(DateTime::Unit::Month, result.ExpireDue + 1);
						temp.SetDay(1);

						expireDate = DateTime(temp, DateTime::Time());
					}
					else if (!result.ExpireDate.empty())
					{
						expireDate = DateTime(result.ExpireDate.c_str());
					}

					if (expireDate != DateTime::Infinite)
					{
						if (expireDate < DateTime::Now())
						{
							event.SendSystemChat(L"$UI.Global.SystemChat.IdentifiedItemIsExpired$");
							return false;
						}
						else
							InstanceEx.ExpireDate = expireDate;
					}

					if (itemInfo->MaxCnt == 1) { //스킬+, 스탯+등은 Stack이 1인 아이템만 적용가능해야 하지 않을까?ㅎㅎ -hak-
						field.CashEnchant1 = result.CashEnchant1Index;
						field.CashEnchant2 = result.CashEnchant2Index;
						field.CashEnchant3 = result.CashEnchant3Index;
						field.CashEnchant4 = result.CashEnchant4Index;
					}

					InstanceEx = field;
				}

				// 인벤토리에서 삭제,
				if (!inventory.DropItem(bag, pos, item->StackedCount))
				{
					ALLM_ERROR((L"Items::Identify() - DropItem() == false"));
					return false;
				}

				if (itemInfo->ItemType != Database::Info::ItemInfo::Type::INSTANT) { // instant item은 인벤에 넣어줄 이유가 없다.
					if (itemInfo->MaxCnt > 1 && XRated::InstanceEx(0)) // it's better to stack
					{
						ItemPosition stackedPosition = AddItem(itemInfo, stackCount, InstanceEx);
						if (stackedPosition == ItemPosition::Invalid)
						{
							event.SendItemMail(itemInfo->Hash, stackCount, InstanceEx);
							return false;
							//					ALLM_ERROR(( L"failed to add identified stackable item" ));
							//					return false;
						}
						else
						{
							bag = stackedPosition.Bag;
							pos = stackedPosition.Position;
						}
					}
					else
					{
						/* recursive auto-identify */
						if (itemInfo->ItemType == Database::Info::ItemInfo::Type::UNIDENTIFIED &&
							Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem(itemInfo->Hash)->AutomaticIdentify)
						{
							if (AddItem(ItemPosition(static_cast<uint8>(bag), static_cast<uint8>(pos)), itemInfo, stackCount, InstanceEx) == false)
							{
								ALLM_ERROR((L"failed to add identified item"));
								return false;
							}
							return Identify(bag, pos, acquiredItem);
						}

						Database::Info::BagInfos::BagType bagType = Database::DatabaseInstance().InfoCollections.BagInfos.GetBagType(bag);
						if ((bagType != Database::Info::BagInfos::BagType::Normal) && !Database::DatabaseInstance().InfoCollections.BagInfos.UsableItem(bagType, itemInfo))
						{
							XRated::ItemPosition emptyPos = inventory.FindEmptySlot(itemInfo);
							if (emptyPos == XRated::ItemPosition::Invalid)
							{
								event.SendItemMail(itemInfo->Hash, stackCount, InstanceEx);
								return false;
							}
							else
							{
								bag = emptyPos.Bag;
								pos = emptyPos.Position;
							}
						}
						if (AddItem(ItemPosition(static_cast<uint8>(bag), static_cast<uint8>(pos)), itemInfo, stackCount, InstanceEx) == false)
						{
							ALLM_ERROR((L"failed to add identified item"));
							return false;
						}
					}

					Protocol::FromServer::AcquireItem acquireitem;
					const Common::ItemEx& identifiedItem = *GetItem(bag, pos);

					acquireitem.ItemId = itemInfo->Hash;
					acquireitem.Bag = static_cast<uint16>(bag);
					acquireitem.Pos = static_cast<uint16>(pos);
					acquireitem.StackedCount = identifiedItem.StackedCount;
					acquireitem.instanceEx = InstanceEx;
					Send(acquireitem);
					/*
					if (event.IsAchievementConnected()) {
						Net::Protocol::Achievement::ServerProtocol::ItemAcquired packet;
						packet.characterName = event.GetName();
						packet.itemHash = itemInfo->Hash;
						packet.count = identifiedItem.StackedCount;
						packet.isEquip = 0;
						packet.equipPart = identifiedItem.Info->EquipParts;
						if(Database::Enchant::IsEnchantable(identifiedItem.Info)){
							Database::Enchant::EnchantBitfields field(instance);
							packet.addItemType = field.LightLevel;
						}else{
							Database::Enchant::NormalBitfields field(instance);
							packet.addItemType = uint32(-1);
						}
						packet.addItemType = uint32(2);
						packet.rarity = identifiedItem.Info->EquipLv;
						event.ItemsSendAchievement(packet);
					}
					*/
					//Log("acquireitem4", identifiedItem );
					//output
					if (acquiredItem != NULL) {
						acquiredItem->Id = itemInfo->Hash;
						acquiredItem->InstanceEx = InstanceEx;
					}

					/* log the result from unidentified box */
					//Logger().Write(IAdmLogger::ImportantLogger, "Identifyitem", event.GetName(), Service::Http::Logger::Parameter()
					//	<< identifiedItem.Info->Hash << identifiedItem.Serial << identifiedItem.StackedCount << identifiedItem.InstanceEx.Instance
					//	<< identifiedItem.InstanceEx.ExpireDate.ToString() << 1/*means a result*/);
				}
				else {
					//Logger().Write(IAdmLogger::ImportantLogger, "Identifyitem", event.GetName(), Service::Http::Logger::Parameter()
					//	<< itemInfo->Hash << 0 << stackCount << 0 << 1/*means a result*/);
				}

				return true;
			}
			bool Items::GetIdentifiedItem(uint32 itemHash, XRated::ItemBasicInfo& resultItem)
			{
				// ��⿣ AutoIdentifyItem �� ���´ٴ� �������� �ڵ�.
				XRated::UnidentifiedItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem(itemHash);
				if (info == NULL) return false;

				/* identify the item */
				std::vector<float> portions;
				std::vector<XRated::UnidentifiedItemInfo::ProbabilityTable> probabilityTable;
				GetProbabilityTable(info, probabilityTable, portions);

				if (portions.empty())
				{
					ALLM_ERROR((L"unable to find probability item"));
					return false;
				}

				Math::RandomByPortion r(portions.begin(), portions.end());
				const XRated::UnidentifiedItemInfo::ProbabilityTable& result(probabilityTable[r.Next()]);

				const Database::Info::ItemInfo* itemInfo(GetItemInfo(result.Id.c_str()));
				uint16 stackCount(result.StackedCount);

				// unable to find the result item info
				if (itemInfo == NULL)
				{
					ALLM_ERROR((L"unable to find item information of id [{}]", result.Id.c_str()));
					return false;
				}

				/* calculate instance fields */
				XRated::InstanceEx InstanceEx(0);

				if (Database::Enchant::IsEnchantable(itemInfo))
				{
					InstanceEx = result.InstanceEx;
					if (InstanceEx == XRated::InstanceEx(0)) {
						//��ȭ.
						Database::Enchant::EnchantBitfields field = 0;
						if (info->EnchantProbabilitys.ReinforcementProbabilitys.empty() == false) {
							Math::RandomByPortion r(info->EnchantProbabilitys.ReinforcementProbabilitys.begin(), info->EnchantProbabilitys.ReinforcementProbabilitys.end());
							size_t level = r.Next();
							if (level <= XRated::Constants::LocaleSpecificSetting::REINFORCEMENT_LEVEL) {
								field.Reinforcement = level;
							}
						}
						//�� ��ȭ.
						if (info->EnchantProbabilitys.LightReinforcementProbabilitys.empty() == false) {
							Math::RandomByPortion r(info->EnchantProbabilitys.LightReinforcementProbabilitys.begin(), info->EnchantProbabilitys.LightReinforcementProbabilitys.end());
							size_t level = r.Next();
							if (level <= XRated::Constants::LocaleSpecificSetting::LIGHTREINFORCEMENT_LEVEL) {
								field.LightLevel = level;
							}
						}
						InstanceEx = static_cast<int64>(field);
					}
				}
				else if (itemInfo->MaxCnt == 1) // unenchantable
				{
					Database::Enchant::NormalBitfields field(0);

					DateTime expireDate(DateTime::Infinite);
					if (result.ExpireMin)
					{
						expireDate = DateTime::Now();
						int minSum = expireDate.Get(DateTime::Unit::Minute) + result.ExpireMin;
						if (minSum / DateTime::Constant::MinutesInAnHour != 0)
							expireDate.Add(DateTime::Unit::Hour, minSum / DateTime::Constant::MinutesInAnHour);
						expireDate.Set(DateTime::Unit::Minute, minSum % DateTime::Constant::MinutesInAnHour);
					}
					else if (result.ExpireDue)
					{
						expireDate = DateTime::Now();
						DateTime::Date temp = expireDate.GetDate();
						temp = temp.Add(DateTime::Unit::Month, result.ExpireDue + 1);
						temp.SetDay(1);

						expireDate = DateTime(temp, DateTime::Time());
					}
					else if (!result.ExpireDate.empty())
					{
						expireDate = DateTime(result.ExpireDate.c_str());
					}

					if (expireDate != DateTime::Infinite)
					{
						if (expireDate < DateTime::Now())
						{
							event.SendSystemChat(L"$UI.Global.SystemChat.IdentifiedItemIsExpired$");
							return false;
						}
						else
						{
							InstanceEx.ExpireDate = expireDate;
						}
					}

					field.CashEnchant1 = result.CashEnchant1Index;
					field.CashEnchant2 = result.CashEnchant2Index;
					field.CashEnchant3 = result.CashEnchant3Index;
					field.CashEnchant4 = result.CashEnchant4Index;


					InstanceEx = field;
				}

				resultItem.ItemHash = itemInfo->Hash;
				resultItem.InstanceEx = InstanceEx;
				resultItem.StackCount = stackCount;

				return true;
			}
			// BMS 2334 by Yong-Eun, Jung
			//bool Items::EquipItem(uint32 pos, const Common::ItemEx& item)
			//{
			//	if(player == NULL || item.Info == NULL)
			//		return	false;

			//	if(item.Info->EquipParts != static_cast<XRated::Constants::EquipParts>(pos))
			//	{
			//		ALLM_ERROR((L"invalid trying to equip an item({}) to part {}", item.Info->Hash, item.Info->EquipParts));
			//		return false;
			//	}

			//	// ������ ��û�� ť�� ����
			//	if(!itemRequirement.RequireDirectlyEquip(item.Info->Hash, pos))
			//	{
			//		ALLM_ERROR(( L"Items::EquipItem() - Error : RequireDirectlyEquip() == false" ));
			//		return false;
			//	}

			//	// ������ ��û
			//	if(!event.ItemsEquip(item.Info, item.Instance, pos))
			//	{
			//		ALLM_ERROR(( L"Items::EquipItem() - Error : event.ItemsEquip() == false" ));
			//		itemRequirement.Clear();
			//		return false;
			//	}

			//}

			bool Items::IsAbleToUse(const Logic::Player* player, const Database::Info::ItemInfo* info, const Database::Info::StageInfo* stageInfo)
			{
				if (player == NULL || info == NULL)
					return false;

				/* validate class */
				if (info->Limits.Class != XRated::Constants::ClassType::AnyClassType && info->Limits.Class != player->GetClassType())
					return false;

				/* validate level */
				if (info->Limits.Level > player->GetLevel() || info->Limits.MaxLevel < player->GetLevel()
					|| info->Limits.PvpLevel > player->GetPvpLevel() || info->Limits.MaxPvpLevel < player->GetPvpLevel()
					|| info->Limits.WarLevel > player->GetWarLevel() || info->Limits.MaxWarLevel < player->GetWarLevel()
					|| info->Limits.RebirthCount > player->GetRebirthCount() || info->Limits.MaxRebirthCount < player->GetRebirthCount()
					|| info->Limits.StoredLevel > player->GetStoredLevel() || info->Limits.MaxStoredLevel < player->GetStoredLevel())
					return false;

				if (stageInfo != NULL && info->Limits.RebirthCount > stageInfo->RebirthCountEquipLimit)
					return false;

				std::vector< std::wstring >::const_iterator cit = info->Limits.StageIds.begin();
				if (stageInfo != NULL)
				{
					bool isUseableStageId = false;
					for (; cit != info->Limits.StageIds.end(); ++cit)
					{
						if (*cit == stageInfo->Id)
						{
							isUseableStageId = true;
						}
					}
					if (!info->Limits.StageIds.empty() && isUseableStageId == false)
						return false;
					cit = info->Limits.UnableStageIds.begin();
					for (; cit != info->Limits.UnableStageIds.end(); ++cit)
					{
						if (*cit == stageInfo->Id)
						{
							return false;
						}
					}
				}


				/* validate basic status */
				int Str, Dex, Int, Vit;
				player->GetBaseStat(Str, Dex, Int, Vit);
				if (info->Limits.Str > Str || info->Limits.Dex > Dex || info->Limits.Int > Int || info->Limits.Vit > Vit)
					return false;
				return true;
			}

			// User::ItemAcquire / User::ItemAdd �� �ϳ��� ��ģ��.
			bool Items::ItemAcquired(const uint32 id, Serial /*gameObjectSerial*/, uint32 count, InstanceEx instance, bool autoStack, XRated::Constants::AddItemType addItemType, uint64 itemSerial, bool addSeparate, XRated::Item* identifiedAcquireItem)
			{
				const Database::Info::ItemInfo* itemInfo = GetItemInfo(id);
				if (itemInfo == NULL)
				{
					ALLM_ERROR((L"Items::ItemAcquired() - itemInfo(id) == NULL"));
					Protocol::FromServer::Error error;
					error.errorcode = Errors::Unexpected;
					error.errorstring = L"������ ȹ�� ���� : ��ϵ��� ���� �������Դϴ�.";
					Send(error);
					return false;
				}

				if (addSeparate == false)
				{
					ItemPosition pos(AddItem(itemInfo, count, instance, autoStack));
					if (pos == ItemPosition::Invalid)
					{
						Protocol::FromServer::Error error;
						error.errorcode = Errors::InventoryFull;
						Send(error);
						return false;
					}

					if (itemInfo->ItemType == Database::Info::ItemInfo::Type::UNIDENTIFIED)
					{
						const XRated::UnidentifiedItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.RetrieveUnidentifiedItem(itemInfo->Hash);
						if (info == NULL) {
							ALLM_EXCEPTION((L"[Items::ItemAcqured] UnidentifiedItemInfo* == NULL ({})", id));
						}
						else {
							if (info->AutomaticIdentify)
								return Identify(pos.Bag, pos.Position, identifiedAcquireItem);
						}
					}

					if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(itemInfo->Hash) == true)
						event.PetItemPositionUpdate(itemSerial, pos.Bag, pos.Position);

					const Common::ItemEx& acquiredItem = *GetItem(pos.Bag, pos.Position);

					if (itemSerial != 0)
						const_cast<Common::ItemEx&>(acquiredItem).Serial = itemSerial;

					Protocol::FromServer::AcquireItem acquireitem;
					acquireitem.ItemId = id;
					acquireitem.Bag = static_cast<uint16>(pos.Bag);
					acquireitem.Pos = static_cast<uint16>(pos.Position);
					acquireitem.StackedCount = acquiredItem.StackedCount;
					acquireitem.instanceEx = instance;
					Send(acquireitem);

					/*
					if (event.IsAchievementConnected()) {
						Net::Protocol::Achievement::ServerProtocol::ItemAcquired packet;
						packet.characterName = event.GetName();
						packet.itemHash = acquiredItem.Info->Hash;
						packet.count = acquiredItem.StackedCount;
						packet.isEquip = 0;
						packet.equipPart = acquiredItem.Info->EquipParts;
						packet.addItemType = addItemType;
						packet.rarity = acquiredItem.Info->EquipLv;
						event.ItemsSendAchievement(packet);
					}
					*/
					//Log("acquireitem2", acquiredItem );

					//switch (addItemType)
					//{
					//case XRated::Constants::AddItemType::Fish:
					//	Logger().Write(IAdmLogger::ImportantLogger, "FishItem", event.GetName(), Service::Http::Logger::Parameter()
					//		<< acquiredItem.Info->Hash << acquiredItem.Serial << count << acquiredItem.Instance );
					//	break;
					//}
				}
				else
				{
					std::vector<XRated::ItemPosition> positions = AddItems(itemInfo, count, instance, autoStack);

					if (positions.empty() == true)
					{
						Protocol::FromServer::Error error;
						error.errorcode = Errors::InventoryFull;
						Send(error);

						return false;
					}

					for (std::vector<XRated::ItemPosition>::const_iterator itr = positions.begin(); itr != positions.end(); ++itr)
					{
						const Common::ItemEx& acquiredItem = *GetItem(itr->Bag, itr->Position);

						if (itemSerial != 0)
							const_cast<Common::ItemEx&>(acquiredItem).Serial = itemSerial;

						Protocol::FromServer::AcquireItem acquireitem;
						acquireitem.ItemId = id;
						acquireitem.Bag = static_cast<uint16>(itr->Bag);
						acquireitem.Pos = static_cast<uint16>(itr->Position);
						acquireitem.StackedCount = acquiredItem.StackedCount;
						acquireitem.instanceEx = instance;

						Send(acquireitem);

						acquireitem.StackedCount = count; // �α׿��� ȹ���� ������ ���� �����.	
						/*
						if (event.IsAchievementConnected()) {
							Net::Protocol::Achievement::ServerProtocol::ItemAcquired packet;
							packet.characterName = event.GetName();
							packet.count = acquiredItem.StackedCount;
							packet.equipPart = acquiredItem.Info->EquipParts;
							packet.addItemType = acquiredItem.Info->ItemType;
							packet.equipPart = acquiredItem.Info->EquipParts == 0 ? uint32(-1) : acquiredItem.Info->EquipParts;
							packet.rarity = acquiredItem.Info->EquipLv;
							packet.itemHash = acquiredItem.Info->Hash;
							//if(acquiredItem.Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USE)
							if(acquiredItem.Info->EquipParts == 0)
								packet.isEquip = 1;
							else
							packet.isEquip = 0;
							//event.ItemsSendAchievement(packet);
						}
						*/
						std::cout << "acquireitem3 -> Please let Teles know what did u do to call this pl0x" << std::endl;
						//Log("acquireitem3", acquiredItem );

						//switch (addItemType)
						//{
						//case XRated::Constants::AddItemType::Fish:
						//	Logger().Write(IAdmLogger::ImportantLogger, "FishItem", event.GetName(), Service::Http::Logger::Parameter()
						//		<< acquiredItem.Info->Hash << acquiredItem.Serial << count << acquiredItem.Instance );
						//	break;
						//}
					}
				}

				return true;
			}

			XRated::ItemPosition Items::AddItem(const Database::Info::ItemInfo* info, uint32 count, InstanceEx instance, bool autoStack)
			{
				if (info == NULL || info->MaxCnt < static_cast<int>(count))
					return XRated::ItemPosition::Invalid;

				XRated::ItemPosition pos(XRated::ItemPosition::Invalid);
				if (autoStack)
					pos = inventory.FindStackableSlot(info, instance, static_cast<uint16>(count));

				if (pos == XRated::ItemPosition::Invalid) // not stackable or no empty slot
				{
					pos = inventory.FindEmptySlot(info);
					if (pos == XRated::ItemPosition::Invalid) return pos; // no empty slot

					if (AddItem(ItemPosition(pos.Bag, pos.Position), info, count, instance))
					{
						return pos;
					}
				}
				else // stackable
					inventory.AddItem(pos.Bag, pos.Position, count);

				return XRated::ItemPosition(pos.Bag, pos.Position); // i really hate this +-1 stufs. how stupid.
			}

			std::vector<XRated::ItemPosition> Items::AddItems(const Database::Info::ItemInfo* info, uint32 count, XRated::InstanceEx instance, bool autoStack)
			{
				std::vector<XRated::ItemPosition> positions;
				XRated::ItemPosition pos(XRated::ItemPosition::Invalid);

				if (info == NULL)
					return positions;

				if (autoStack)
				{
					positions = inventory.FindStackableSlots(info, instance, static_cast<uint16>(count), false);

					for (std::vector<XRated::ItemPosition>::iterator itr = positions.begin(); itr != positions.end(); ++itr)
					{
						//++itr->Bag;
						const Common::ItemEx* item = GetItem(itr->Bag, itr->Position, false);

						/// ���ӹ� ���׽�Ʈ �� �ؾ��Թ�?��,.��;; �ȳ��ڴ� ���� �Ҿ��ϳ�
						if (item != NULL)
						{
							uint32 addCount = info->MaxCnt < count ? info->MaxCnt : count;

							if (item->Info != NULL && info->MaxCnt < addCount + item->StackedCount)
								addCount -= addCount + item->StackedCount - info->MaxCnt;

							if (inventory.AddItem(itr->Bag, itr->Position, addCount) == true)
								count -= addCount;
						}
					}
				}

				while (count != 0)
				{
					uint32 addCount = info->MaxCnt < count ? info->MaxCnt : count;

					pos = inventory.FindEmptySlot(info);
					if (pos == XRated::ItemPosition::Invalid)
						return positions;

					/*pos.Bag = pos.Bag + 1;*/

					if (AddItem(pos, info, addCount, instance) == true)
					{
						count -= addCount;
						positions.push_back(pos);
					}
				}

				return positions;
			}

			XRated::ItemPosition Items::AddItem(const Common::ItemEx& item)
			{
				ItemPosition pos(inventory.FindEmptySlot(item.Info));
				if (!(pos == ItemPosition::Invalid))
				{
					inventory.SetItem(pos.Bag, pos.Position, item);
				}
				return pos;
			}

			bool Items::AddItem(XRated::ItemPosition pos, const Database::Info::ItemInfo* info, uint32 count, InstanceEx instance)
			{
				Common::ItemEx item;
				item.Info = info;
				item.Serial = ItemSerialInstance().MakeItemSerial();
				item.InstanceEx = instance;
				item.StackedCount = static_cast<uint16>(count);

				return inventory.SetItem(pos.Bag, pos.Position, item);
			}

			bool Items::IsItemAddable(const Database::Info::ItemInfo* info, InstanceEx instance, uint32 count, bool autoStack) const
			{
				XRated::ItemPosition pos(XRated::ItemPosition::Invalid);

				if (autoStack)
				{
					std::vector<XRated::ItemPosition> positions = inventory.FindStackableSlots(info, instance, static_cast<uint16>(count), false);

					return !positions.empty();
				}

				if (pos == XRated::ItemPosition::Invalid) // not stackable or no empty slot
				{
					pos = inventory.FindEmptySlot(info);
					if (pos == XRated::ItemPosition::Invalid)
						return false; // no empty slot
				}

				return true;
			}

			uint32 Items::CountRemainStackableItem(const Database::Info::ItemInfo* info, InstanceEx instance, uint32 count) const
			{
				return inventory.CountRemainStackableItem(info->Hash, instance, count);
			}

			uint32 Items::CountRemainAfterAddItem(const Database::Info::ItemInfo* info, InstanceEx instance, uint32 count, uint32 maxCnt) const
			{
				return inventory.CountRemainAfterAddItem(info->Hash, instance, count, maxCnt);
			}

			bool Items::ItemUsed(uint32 returnValue/*Logic::Player::ItemUseResult*/, const Database::Info::ItemInfo* info, uint32 bag, uint32 pos)
			{
				if (itemRequirement.Result() == NULL)
				{
					ALLM_IMPORTANT((L"Items::ItemUsed() - result == NULL"));
					return false;
				}

				class Releaser ///< make sure to do 'itemRequirement.Clear()' at the end of this stack. itemRequirement.Clear() should not work before 'DropItems()'
				{
					ItemRequirementQue& r;
				public:
					Releaser(ItemRequirementQue& r) : r(r) {}
					~Releaser() { r.Clear(); }
				} releaser(itemRequirement);

				IItemResult& result = *(itemRequirement.Result());

				if (result.Kind() != USEITEM && result.Kind() != USEITEMINPETINVEN)
				{
					ALLM_ERROR((L"User::ItemUsed() - result.Kind({}) != USEITEM or != USEITEMINPETINVEN", result.Kind()));
					return false;
				}

				if (returnValue != Logic::Player::ItemUseResult::Success)
				{
					switch (returnValue)
					{
					case Logic::Player::ItemUseResult::FailInventoryCoolDownNow:
					case Logic::Player::ItemUseResult::FailItemCoolDownNow:
					{
						Protocol::FromServer::UseItem useitem;
						useitem.playerserial = -1;
						useitem.itemid = 0;

						Send(useitem);
					}
					break;
					default:
					{
						ALLM_ERROR((L"User::ItemUsed() - bUsed != Logic::Player::ItemUseResult::Success"));
					}
					break;
					}

					return false;
				}

				if (info == NULL)
				{
					ALLM_ERROR((L"User::ItemUsed() - info == NULL"));
					return false;
				}
				if ((result.Used_Bag() != bag) || (result.Used_Pos() != pos))
				{
					ALLM_ERROR((L"User::ItemUsed() - used({},&d) != use({},{})", bag, pos, result.Used_Bag(), result.Used_Pos()));
					return false;
				}

				if (result.Kind() == USEITEM)
				{
					const Common::ItemEx* item = GetItem(bag, pos);
					if (item == NULL)
					{
						ALLM_ERROR((L"User::ItemUsed() - already item has been gone)"));
						return false;
					}
					else
					{
						Common::ItemEx itemForLog(*item);

						itemForLog.StackedCount = 1;

						Log("useitem", itemForLog);
					}

					// ��� �� �Ҹ�Ǵ� �������� �ƴ� ��� Drop ó�� �� �ʿ䰡 ����.
					if (!(info->GetAttributes() & Database::Info::ItemInfo::ATTRIBUTES::CONSUME))
					{
						return true;
					}

					if (!DropItems(bag, pos, 1, true))
					{
						ALLM_ERROR((L"User::ItemUsed() - DropItems() == false"));
						return false;
					}
				}
				else
				{
					XRated::PetItemSlot::PositionType type = static_cast<XRated::PetItemSlot::PositionType>(bag);
					const Common::ItemEx* item = petsInventory.GetItem(result.Used_ExInfo(), type, pos);
					if (item == NULL)
					{
						ALLM_ERROR((L"User::ItemUsed() - already item has been gone)"));
						return false;
					}
					else
					{
						Common::ItemEx itemForLog(*item);

						itemForLog.StackedCount = 1;

						Log("useitem", itemForLog);
					}

					// ��� �� �Ҹ�Ǵ� �������� �ƴ� ��� Drop ó�� �� �ʿ䰡 ����.
					if (!(info->GetAttributes() & Database::Info::ItemInfo::ATTRIBUTES::CONSUME))
					{
						return true;
					}

					if (!petsInventory.DropItem(result.Used_ExInfo(), type, pos, 1))
					{
						ALLM_ERROR((L"User::ItemUsed() - DropItems() == false"));
						return false;
					}

					Protocol::FromServer::DropItemInPetInventory packet;

					packet.PetSerial = result.Used_ExInfo();
					packet.PositionType = type;
					packet.Position = pos;
					packet.StackCount = item->StackedCount;

					Send(packet);
				}

				/*XRated::PlayerData& data = player->GetPlayerData();
				auto user = std::static_pointer_cast<User>(data.user);
				if (user->achievement.IsConnected()) {
					Net::Protocol::Achievement::ServerProtocol::ItemUsed packet;
					packet.characterName = user->GetName();
					packet.itemHash = uint32(info->Hash);
					packet.marketCategory = uint32(info->MarketCategory);
					user->achievement.Send(packet);
				}
				std::cout << "[Protocol::Achievement::ServerProtocol::ItemUsed]: [ " << info->Hash << "] [" << info->MarketCategory << "]" << std::endl;
				*/
				return true;
			}


			uint16 Items::ItemEquiped(bool succeed) // from user (from logic)
			{
				class Releaser
				{
				public:
					Releaser(ItemRequirementQue& r) : r(r) { }
					~Releaser() { r.Clear(); }

				private:
					ItemRequirementQue& r;

				} r(itemRequirement);


				if (itemRequirement.Result() == NULL)
				{
					ALLM_IMPORTANT((L"Items::ItemEquiped() - result == NULL"));
					return Constants::InvalidPosition;
				}

				if (!succeed)
				{
					return Constants::InvalidPosition;
				}

				IItemResult& result = *(itemRequirement.Result());
				switch (result.Kind())
				{
				case DROPEQUIP:
					return EquipDroped(result);

				case MOVEEQUIP:
					// ��� ����(�Ա�)
					if (result.Equiped_Equip())
					{
						return EquipPutOned(result);
					}
					// ��� ����(����)
					else // Equip -> Inventory
					{
						return EquipTakeOffed(result);
					}
				case PETMOVEEQUIP:
					// �� ��� ����(�Ա�)
					if (result.Equiped_Equip())
					{
						return PetEquipPutOned(*itemRequirement.PetEquipResult());
					}
					// �� ��� ����(����)
					else // Equip -> Inventory
					{
						return PetEquipTakeOffed(*itemRequirement.PetEquipResult());
					}
					break;
				case PETEQUIPMOVEINPETINVEN:
					return PetInventoryToEquiped(result);

					// BMS 2334 by Yong-Eun, Jung
				case DIRECTLYEQUIP:
					return DirectlyEquiped(result);



					//case EQUIPTOEQUIP:
					//	{
					//		int sPos=result.Equiped_EPos(), tPos=result.Equiped_IPos();
					//		Common::ItemEx s, t; s.Info=NULL; t.Info=NULL;

					//		if (equip.GetItem(sPos)!=NULL) s=*equip.GetItem(sPos);
					//		if (equip.GetItem(tPos)!=NULL) t=*equip.GetItem(tPos);

					//		/* validation */
					//		if (s.Info!=NULL && XRated::Constants::IsEquippable(static_cast<XRated::Constants::Equipment>(tPos), s.Info->EquipParts)==false )
					//			return false;
					//		if (t.Info!=NULL && XRated::Constants::IsEquippable(static_cast<XRated::Constants::Equipment>(sPos), t.Info->EquipParts)==false )
					//			return false;

					//		equip.PushItems(sPos, t);
					//		equip.PushItems(tPos, s);
					//		break;
					//	}

				default:
					ALLM_ERROR((L"Items::ItemEquiped() - Error : Kind({})", result.Kind()));
					break;
				}

				return Constants::InvalidPosition;
			}
			void Items::ItemEquipToEquiped(bool succeed)
			{
				class Releaser
				{
				public:
					Releaser(ItemRequirementQue& r) : r(r) { }
					~Releaser() { r.Clear(); }

				private:
					ItemRequirementQue& r;

				} r(itemRequirement);

				if (succeed == true) {
					if (itemRequirement.Result() == NULL)
					{
						ALLM_IMPORTANT((L"Items::ItemEquiped() - result == NULL"));
						return;
					}


					IItemResult& result = *(itemRequirement.Result());

					if (result.Kind() == EQUIPTOEQUIP) {
						EquipToEquiped(result);
					}
					else {
						ALLM_IMPORTANT((L"Items::ItemEquiped() - result is not EquipToEquip Type"));
					}
				}
			}

			void Items::EquipmentSwapped(bool succeed)
			{
				class Releaser
				{
				public:
					Releaser(ItemRequirementQue& r) : r(r) { }
					~Releaser() { r.Clear(); }

				private:
					ItemRequirementQue& r;

				} r(itemRequirement);

				if (succeed == true) {
					if (itemRequirement.Result() == NULL)
					{
						ALLM_IMPORTANT((L"Items::ItemEquiped() - result == NULL"));
						Protocol::FromServer::EquipmentSwapped packet;
						packet.Result = Protocol::FromServer::EquipmentSwapped::Error;
						Send(packet);
						return;
					}

					IItemResult& result = *(itemRequirement.Result());

					if (result.Kind() == SWAPEQUIP) {
						event.SetEquipmentIndex(result.Equiped_SetIndex());
						SwapNewEquipment(result);
					}
					else {
						ALLM_IMPORTANT((L"Items::ItemEquiped() - result is not EquipToEquip Type"));
						Protocol::FromServer::EquipmentSwapped packet;
						packet.Result = Protocol::FromServer::EquipmentSwapped::Error;
						Send(packet);
					}
				}
				else
				{
					Protocol::FromServer::EquipmentSwapped packet;
					packet.Result = Protocol::FromServer::EquipmentSwapped::Error;
					Send(packet);
				}
			}

			void Items::CashEquipmentSwapped(bool succeed)
			{
				class Releaser
				{
				public:
					Releaser(ItemRequirementQue& r) : r(r) { }
					~Releaser() { r.Clear(); }

				private:
					ItemRequirementQue& r;

				} r(itemRequirement);

				if (succeed == true) {
					if (itemRequirement.Result() == NULL)
					{
						ALLM_IMPORTANT((L"Items::ItemEquiped() - result == NULL"));
						Protocol::FromServer::CashEquipmentSwapped packet;
						packet.Result = Protocol::FromServer::CashEquipmentSwapped::Error;
						Send(packet);
						return;
					}

					IItemResult& result = *(itemRequirement.Result());

					if (result.Kind() == SWAPCASHEQUIP) {
						event.SetCashEquipmentIndex(result.Equiped_SetIndex());
						SwapNewEquipment(result);
					}
					else {
						ALLM_IMPORTANT((L"Items::ItemEquiped() - result is not EquipToEquip Type"));
						Protocol::FromServer::CashEquipmentSwapped packet;
						packet.Result = Protocol::FromServer::CashEquipmentSwapped::Error;
						Send(packet);
					}
				}
				else
				{
					Protocol::FromServer::CashEquipmentSwapped packet;
					packet.Result = Protocol::FromServer::CashEquipmentSwapped::Error;
					Send(packet);
				}
			}




			uint16 Items::EquipDroped(IItemResult& result)
			{
				// ���â �˻�
				uint16 pos = static_cast<uint16>(result.Dropd_Pos());
				const Common::ItemEx* pEquipItem = equip.GetItem(pos);
				if (pEquipItem == NULL)
				{
					ALLM_ERROR((L"User::EquipDroped() - Error : pEquipItem == NULL"));
					itemRequirement.Clear();
					return Constants::InvalidPosition;
				}

				{
					Common::ItemEx itemForLog = *pEquipItem;
					itemForLog.StackedCount = 1;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
					Log("dropitem", itemForLog);
				}

				// �ϴ� ���â���� �����Ѵ�.
				equip.DeleteItem(result.Dropd_Pos());

				Protocol::FromServer::DropItem dropItem;
				dropItem.bag = 0;
				dropItem.pos = pos;
				dropItem.DroppedCount = 1;
				dropItem.StackedCount = 0;
				Send(dropItem);

				itemRequirement.Clear();

				return pos;
			}


			uint16 Items::EquipPutOned(IItemResult& result)
			{
				// �κ� â �˻�
				const Common::ItemEx* pInvenItem = inventory.GetItem(result.Equiped_IBag(), result.Equiped_IPos());
				if (pInvenItem == NULL)
				{
					ALLM_ERROR((L"User::EquipPutOned() - Error : pInvenItem == NULL"));
					itemRequirement.Clear();
					return Constants::InvalidPosition;
				}



				/* TODO : validation for empty slot count for one-piece equipment */
				Common::ItemEx invenItem = *pInvenItem; // �̰� �����ؾ� �Ѵ�.
				if (Database::Enchant::IsEnchantable(invenItem.Info)
					&& !(invenItem.Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEAFTERUSE)) // update HasEquipped state, check TRADEAFTERUSE attribute to keep compatibility for nerf
					reinterpret_cast<Database::Enchant::EnchantBitfields&>(invenItem.InstanceEx.Instance).HasEquipped = 1;
				else if (Database::Enchant::IsCashEnchantable(*invenItem.Info)
					&& !(invenItem.Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEAFTERUSE)) // update HasEquipped state, check TRADEAFTERUSE attribute to keep compatibility for nerf
					reinterpret_cast<Database::Enchant::NormalBitfields&>(invenItem.InstanceEx.Instance).HasEquipped = 1;

				// �ϴ� �κ����� ����
				inventory.DeleteItem(result.Equiped_IBag(), result.Equiped_IPos());

				// ���â �˻�
				const Common::ItemEx* pEquipItem = equip.GetItem(result.Equiped_EPos());
				//const Database::Info::ItemInfo* invenItemInfo=Database::DatabaseInstance().InfoCollections.Items.Retrieve(invenItem.Id);
				Protocol::FromServer::MoveItem moveitem;
				moveitem.pbag = 0; // from equipment

				//if (invenItemInfo->OnePiece.empty()) // normal equipment
				//{
				if (pEquipItem == NULL) // �� ���â�̸�
				{
					// �׳� ������
					equip.PushItems(result.Equiped_EPos(), invenItem);
				}
				else // ���� ����ִٸ�
				{
					if (pEquipItem->Info != NULL)
						inventory.SetItem(result.Equiped_IBag(), result.Equiped_IPos(), *pEquipItem); // this should be done before equip process because pEquiopItem changes right after equip (equip.PushItems) process

					equip.PushItems(result.Equiped_EPos(), invenItem);

				}

				moveitem.ppos = static_cast<uint16>(result.Equiped_EPos());
				moveitem.nbag = static_cast<uint16>(result.Equiped_IBag());
				moveitem.npos = static_cast<uint16>(result.Equiped_IPos());
				Send(moveitem);

				itemRequirement.Clear();
				return moveitem.ppos;
			}


			uint16 Items::EquipTakeOffed(IItemResult& result)
			{
				// ���â �˻�
				const Common::ItemEx* pEquipItem = equip.GetItem(result.Equiped_EPos());
				if (pEquipItem == NULL)
				{
					ALLM_ERROR((L"User::EquipPutOffed() - Error : pEquipItem == NULL"));
					itemRequirement.Clear();
					return Constants::InvalidPosition;
				}

				Common::ItemEx equipItem = *pEquipItem; // �̰� �κ��� �־�� ��

				// �ϴ� ���â���� �����Ѵ�.
				equip.DeleteItem(result.Equiped_EPos());

				// �κ� â �˻�
				const Common::ItemEx* pInvenItem = inventory.GetItem(result.Equiped_IBag(), result.Equiped_IPos());

				// ���... ���� �� ����ֳ�
				if (pInvenItem != NULL)
				{
					ALLM_ERROR((L"User::EquipPutOffed() - pInvenItem({}) != NULL", pInvenItem->Info->Hash));
					itemRequirement.Clear();
					return Constants::InvalidPosition;
				}

				inventory.SetItem(result.Equiped_IBag(), result.Equiped_IPos(), equipItem);

				Protocol::FromServer::MoveItem moveitem;
				moveitem.pbag = 0;
				moveitem.ppos = static_cast<uint16>(result.Equiped_EPos());
				moveitem.nbag = static_cast<uint16>(result.Equiped_IBag());
				moveitem.npos = static_cast<uint16>(result.Equiped_IPos());
				Send(moveitem);

				itemRequirement.Clear();
				return moveitem.ppos;
			}
			void Items::EquipToEquiped(IItemResult& result)
			{
				// ���â �˻�
				equip.MoveItems(result.Equiped_IPos(), result.Equiped_EPos());

				Protocol::FromServer::MoveItem moveitem;
				moveitem.pbag = 0;
				moveitem.ppos = static_cast<uint16>(result.Equiped_EPos());
				moveitem.nbag = 0;
				moveitem.npos = static_cast<uint16>(result.Equiped_IPos());
				Send(moveitem);

				itemRequirement.Clear();
			}
			void Items::SwapNewEquipment(IItemResult& result)
			{
				itemRequirement.Clear();
			}


			uint16 Items::PetInventoryToEquiped(IItemResult& result)
			{
				const Common::ItemEx* sourceItem = petsInventory.GetItem(result.Used_ExInfo(), PetItemSlot::PositionType::Inventory, result.Equiped_IPos());
				const Common::ItemEx* targetItem = petsInventory.GetItem(result.Used_ExInfo(), PetItemSlot::PositionType::Equipment, result.Equiped_EPos());

				Common::ItemEx source;
				if (sourceItem != NULL)
					source = *sourceItem;
				else
					source.Info = NULL;

				Common::ItemEx target;
				if (targetItem != NULL)
					target = *targetItem;
				else
					target.Info = NULL;

				////////////////// Need PetInvenValidation - �� ���â�� ����Ϸ��� �ϴ°� �������̼��ؾ���.
				if (petsInventory.SetItem(result.Used_ExInfo(), PetItemSlot::PositionType::Equipment, result.Equiped_EPos(), source) == false)
				{
					ALLM_ERROR((L"Items::PetInventoryToEquiped() Error - SetItem(sourceSlot)"));
					return Items::Constants::InvalidPosition;
				}
				if (petsInventory.SetItem(result.Used_ExInfo(), PetItemSlot::PositionType::Inventory, result.Equiped_IPos(), target) == false)
				{
					ALLM_ERROR((L"Items::PetInventoryToEquiped() Error - SetItem(targetSlot)"));

					/// �ѹ�? �̰� �� ����� Yong-Eun, Jung 2008.06.10
					petsInventory.SetItem(result.Used_ExInfo(), PetItemSlot::PositionType::Equipment, result.Equiped_EPos(), source);
					return Items::Constants::InvalidPosition;
				}

				Protocol::FromServer::MoveItemInPetInven sendPacket;

				sendPacket.PetSerial = result.Used_ExInfo();
				sendPacket.SourcePositionType = PetItemSlot::PositionType::Equipment;
				sendPacket.SourcePosition = result.Equiped_EPos();
				sendPacket.TargetPositionType = PetItemSlot::PositionType::Inventory;
				sendPacket.TargetPosition = result.Equiped_IPos();

				Send(sendPacket);

				return result.Equiped_EPos();
			}

			// BMS 2334 by Yong-Eun, Jung
			uint16 Items::DirectlyEquiped(IItemResult& result)
			{
				const Common::ItemEx* pEquipItem = equip.GetItem(result.Equiped_EPos());
				Common::ItemEx bankItem;
				bankItem.Info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(result.Equiped_EID());
				bankItem.InstanceEx = result.Equiped_EIns();
				bankItem.Serial = result.Equiped_ESerial();
				bankItem.StackedCount = 1;

				if (Database::Enchant::IsEnchantable(bankItem.Info)
					&& !(bankItem.Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEAFTERUSE)) // update HasEquipped state, check TRADEAFTERUSE attribute to keep compatibility for nerf
					reinterpret_cast<Database::Enchant::EnchantBitfields&>(bankItem.InstanceEx.Instance).HasEquipped = 1;
				else if (Database::Enchant::IsCashEnchantable(*bankItem.Info)
					&& !(bankItem.Info->Attributes.Attr & XRated::Database::Info::ItemInfo::ATTRIBUTES::TRADEAFTERUSE)) // update HasEquipped state, check TRADEAFTERUSE attribute to keep compatibility for nerf
					reinterpret_cast<Database::Enchant::NormalBitfields&>(bankItem.InstanceEx.Instance).HasEquipped = 1;

				// �������
				equip.DeleteItem(result.Equiped_EPos());
				// ����
				equip.PushItems(result.Equiped_EPos(), bankItem);
				itemRequirement.Clear();

				return static_cast<uint16>(result.Equiped_EPos());
			}

			void Items::GetEquip(std::vector<Common::ItemEx>& equipments) const
			{
				equip.GetEquip(equipments);
			}

			uint16 Items::Remove(const XRated::ItemPosition& pos, uint32 itemHash, uint16 count, bool sendPacket, bool writeLog)
			{
				const Common::ItemEx* item = inventory.GetItem(pos.Bag, pos.Position);
				if (item != NULL)
				{
					uint16 stackedCnt = item->StackedCount;
					if (pos.Bag == 0)
					{
						// by jiff100. Unable to drop equipment with this method
						return 0;
					}
					else
					{
						const Common::ItemEx* item = inventory.GetItem(pos.Bag, pos.Position);
						if (item == NULL) return false; // should not be.

						if (item->StackedCount > count)
						{
							if (writeLog)
							{
								Common::ItemEx itemForLog = *item;
								itemForLog.StackedCount = count;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
								Log("dropitem", itemForLog);
							}

							if (inventory.DropItem(pos.Bag, pos.Position, count) == false)
								return 0;
						}
						else
						{
							if (writeLog)
							{
								Common::ItemEx itemForLog = *item;
								itemForLog.StackedCount = count;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
								Log("dropitem", itemForLog);
							}

							count = item->StackedCount;
							//count-=droppedCount;
							inventory.DeleteItem(pos.Bag, pos.Position);
						}

						//if ( inventory.DropItem(pos.Bag, pos.Position, count) == false )
						//	return 0;
					}

					if (sendPacket)
					{
						Protocol::FromServer::DropItem dropItem;
						dropItem.bag = static_cast<uint16>(pos.Bag);
						dropItem.pos = static_cast<uint16>(pos.Position);
						dropItem.DroppedCount = static_cast<uint16>(count);
						dropItem.StackedCount = (stackedCnt > count ? stackedCnt - count : 0);
						Send(dropItem);
					}

					return count;
				}

				// danpat04. ������ �� �� ������ ��ǥ�� ã�� ���� �״� ��찡 �ִ�
				//			 ���߿� �� ��Ȳ�� ���ؼ� �� �� ������ �ʿ��ҵ� �ϴ�
				return 0;
			}

			bool Items::Remove(uint32 itemHash, uint32 count)
			{
				if (inventory.GetItemCount(itemHash) + GetItemCountInPetInventory(itemHash, true) < count) return false; // not enough item

				uint32 droppedCount(0);
				while (count)
				{
					ItemPosition pos = inventory.FindItem(itemHash);
					if (pos != ItemPosition::Invalid)
					{
						const Common::ItemEx* item = inventory.GetItem(pos.Bag, pos.Position);
						if (item == NULL) return false; // should not be.

						if (item->StackedCount > count)
						{
							droppedCount = count;
							count = 0;

							{
								Common::ItemEx itemForLog = *item;
								itemForLog.StackedCount = droppedCount;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
								Log("dropitem", itemForLog);
							}

							inventory.DropItem(pos.Bag, pos.Position, droppedCount);
						}
						else
						{
							droppedCount = item->StackedCount;
							count -= droppedCount;

							{
								Common::ItemEx itemForLog = *item;
								itemForLog.StackedCount = droppedCount;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
								Log("dropitem", itemForLog);
							}

							inventory.DeleteItem(pos.Bag, pos.Position);
						}

						Protocol::FromServer::DropItem dropItem;
						dropItem.bag = static_cast<uint16>(pos.Bag);
						dropItem.pos = static_cast<uint16>(pos.Position);
						dropItem.DroppedCount = static_cast<uint16>(droppedCount);
						dropItem.StackedCount = (item->Info ? item->StackedCount : 0);
						Send(dropItem);
					}
					else
					{
						Common::PetItemSlotEx slot = FindItemInPetInventory(itemHash, true);

						if (slot.Type == XRated::PetItemSlot::Invaild)
							return false; // actually should not be, because of validation at top of this method.

						const Common::ItemEx* item = petsInventory.GetItem(slot.PetSerial, slot.Type, slot.Position);

						if (item == NULL || item->Info == NULL)
							return false;

						Protocol::FromServer::DropItemInPetInventory packet;

						packet.PetSerial = slot.PetSerial;
						packet.PositionType = slot.Type;
						packet.Position = slot.Position;
						packet.StackCount = 0;

						if (slot.Stacked > count)
						{
							droppedCount = count;
							count = 0;

							{
								Common::ItemEx itemForLog = *item;
								itemForLog.StackedCount = droppedCount;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
								Log("dropitem", itemForLog);
							}

							petsInventory.DropItem(slot.PetSerial, slot.Type, slot.Position, droppedCount);

							packet.StackCount = slot.Stacked - droppedCount;
						}
						else
						{
							droppedCount = slot.Stacked;
							count -= droppedCount;

							{
								Common::ItemEx itemForLog = *item;
								itemForLog.StackedCount = droppedCount;	// �α׿��� ������ ���� ���ܾ��Ѵ�.
								Log("dropitem", itemForLog);
							}

							petsInventory.DropItem(slot.PetSerial, slot.Type, slot.Position);
						}

						Send(packet);
					}
				}

				return true;
			}

			bool Items::RemoveStageItems()
			{
				return RemoveItemsByAttribute(Database::Info::ItemInfo::ATTRIBUTES::STAGEITEM, false, false);
			}

			bool Items::RemoveItemsByAttribute(uint16 attribute, bool checkDisabledBag, bool writeWebLog)
			{
				struct ItemAttribute : public Common::ItemEx::IFindCondition
				{
					uint16 attribute;

					ItemAttribute(uint16 attr) { attribute = attr; }
					bool operator()(const Common::ItemEx& rhs) const
					{
						if (rhs.Info == NULL) return false;
						if (rhs.Info->Attributes.Attr & attribute) return true;
						return false;
					}
				};

				bool ret(false); // indicator for if the stage item exists or not
				uint32 equipSize = XRated::Constants::Equipment::Cnt;

				//���.
				for (uint32 i = 0; i < equipSize; ++i)
				{
					const Common::ItemEx* item = equip.GetItem(i);
					if (item == NULL) continue;
					if (item->Info->Attributes.Attr & attribute)
					{
						ALLM_INFO((L"[Items::RemoveItemsByAttribute] Trying to remove item from equip. ({})", (int)item->Info->Hash));

						/* this packet needed because equip relative packet doesn't affect to players item system */
						Protocol::FromServer::DropItem response;
						response.bag = 0; // equip
						response.pos = static_cast<uint16>(i);
						response.DroppedCount = 1;
						response.StackedCount = 0;
						Send(response);

						event.ItemsEquip(NULL, 0, i);
						equip.DeleteItem(i);
						ret = true;
						//log
						if (writeWebLog == true)
						{
							Common::ItemEx itemForLog = *item;

							itemForLog.StackedCount = 1;	// �α׿��� ������ ���� ���ܾ��Ѵ�.

							Log("dropitem", itemForLog);
						}
					}
				}

				//�κ��丮
				for (ItemPosition pos = inventory.FindItemPosition(ItemAttribute(attribute), checkDisabledBag); !(pos == ItemPosition::Invalid); pos = inventory.FindItemPosition(ItemAttribute(attribute), checkDisabledBag))
				{
					const Common::ItemEx* item = inventory.GetItem(pos.Bag, pos.Position);
					if (item == NULL) break;

					ALLM_INFO((L"[Items::RemoveItemsByAttribute] Trying to remove item from Inven. ({})", (int)item->Info->Hash));

					Protocol::FromServer::DropItem response;

					response.bag = pos.Bag;
					response.pos = pos.Position;
					response.DroppedCount = item->StackedCount; // all of it
					response.StackedCount = 0;
					Send(response);

					DropItem(pos);
					//log
					if (writeWebLog == true)
					{
						Log("dropitem", *item);
					}
					ret = true;
				}
				//����.
				for (ItemPosition pos = bank.FindItemPosition(ItemAttribute(attribute), checkDisabledBag); !(pos == ItemPosition::Invalid); pos = bank.FindItemPosition(ItemAttribute(attribute), checkDisabledBag))
				{
					const Common::ItemEx* item = bank.GetItem(pos.Bag, pos.Position);
					if (item == NULL) break;

					ALLM_INFO((L"[Items::RemoveItemsByAttribute] Trying to remove item from Inven. ({})", (int)item->Info->Hash));

					Protocol::FromServer::DropBankItem response;

					response.position.Bag = pos.Bag;
					response.position.Position = pos.Position;
					Send(response);

					DropBankItems(pos.Bag, pos.Position);
					//log
					if (writeWebLog == true)
					{
						Log("dropitem", *item);
					}
					ret = true;
				}

				//�� �κ��丮.
				for (Common::PetItemSlotEx slot = petsInventory.FindItemPosition(ItemAttribute(attribute)); slot.Type != PetItemSlot::Invaild; slot = petsInventory.FindItemPosition(ItemAttribute(attribute)))
				{
					ALLM_INFO((L"[Items::RemoveItemsByAttribute] Trying to remove item from Pet Inven. ({})", (int)slot.ItemInfo->Hash));

					Protocol::FromServer::DropItemInPetInventory response;

					response.PetSerial = slot.PetSerial;
					response.PositionType = slot.Type;
					response.Position = slot.Position;
					Send(response);

					petsInventory.DropItem(slot.PetSerial, slot.Type, slot.Position);

					//log
					if (writeWebLog == true) {
						Common::ItemEx itemEx;
						itemEx.Info = slot.ItemInfo;
						itemEx.InstanceEx = slot.InstanceEx;
						itemEx.Serial = slot.Serial;
						itemEx.StackedCount = slot.Stacked;
						Log("dropitem", itemEx);
					}
					ret = true;
				}

				return ret;
			}

			int Items::GetRequiredSlotCount(const std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> >& toRemove, const std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> >& toAdd, uint32 availablecount) const
			{
				int requiredSlotCount(0);

				std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> >::const_iterator i;
				std::map<const Database::Info::ItemInfo*, uint32/*required*/> requiredItems;

				/* to make sure no duplicated items in toAdd parameter */
				for (i = toAdd.begin(); i != toAdd.end(); ++i)
				{
					const Database::Info::ItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(i->first);
					if (info != NULL)
						requiredItems[info] += i->second * availablecount; // edit by kds218
					else
					{
						ALLM_ERROR((L"invalid item required ({})", i->first));
					}
				}

				/* calculate the required number of slots for something to be added */
				for (std::map<const Database::Info::ItemInfo*, uint32>::const_iterator reqI = requiredItems.begin(); reqI != requiredItems.end(); ++reqI)
				{
					if (reqI->second == 0) continue; // calculation is not needed
					int count = inventory.GetCount(Common::ItemEx::CountCondition(reqI->first->Hash));
					if (count == 0)
						requiredSlotCount += (int((reqI->second - 1) / reqI->first->MaxCnt) + 1);
					else
					{
						std::vector<const Common::ItemEx*> items(inventory.GetItems(Common::ItemEx::FindCondition((reqI->first->Hash))));
						int required = (reqI->second + count) - (items.size() * reqI->first->MaxCnt); // total(inventory+toAdd) number of the items - total capacity
						if (required > 0)
							requiredSlotCount += (int((required - 1) / reqI->first->MaxCnt) + 1);
					}
				}

				/* calculate removing slot count - this should have same rule as 'Remove()' method and filledSlotCount should be negative(-) */
				for (i = toRemove.begin(); i != toRemove.end(); ++i)
				{
					std::vector<const Common::ItemEx*> items(inventory.GetItemsInNormalBag(Common::ItemEx::FindCondition(i->first)));
					uint32 count(i->second * availablecount); //edit by kds218
					for (std::vector<const Common::ItemEx*>::const_iterator item = items.begin(), end = items.end(); item != end; ++item)
					{
						if (count >= (*item)->StackedCount)
						{
							count -= (*item)->StackedCount;
							--requiredSlotCount;
						}
						else
						{
							/* not enough item to be removed */
							break;
						}
					}
				}

				return requiredSlotCount;
			}


			uint32 Items::GetEmptySlotCount() const
			{
				return inventory.GetEmptySlotCount();
			}

			uint32 Items::GetBlankSlotCountOfNormalBag() const
			{
				return inventory.GetBlankSlotCountOfNormalBag();
			}

			void Items::AllItems(std::vector< Common::ItemSlotEx >& allItems)
			{
				AutoLock lock(cs);
				equip.AllItems(allItems); // add EquipItem to AllItems
				inventory.AllItems(allItems); // add inventoryItem to  allItems
				bank.AllItems(allItems); // add bankItem to  allItems
			}
			//void Items::GetAllItems(std::vector< Common::ItemSlotEx >&equipSlots, std::vector< Common::ItemSlotEx >& inventorySlots, std::vector< Common::ItemSlotEx >& bankSlots )
			//{
			//	AutoLock lock(cs);

			//	equip.AllItems( equipSlots );
			//	inventory.AllItems( inventorySlots );
			//	bank.AllItems( bankSlots );
			//}
			void Items::AllChangedItemsAndOriginBackup(ChangedItems& changedItems, PetsInventory::ChangedItems& petsChangedItems, std::vector< Common::PetItemSlotEx >& petsAllItems
				, std::vector< Common::ItemSlotEx >& equipSlots, std::vector< Common::ItemSlotEx >& inventorySlots, std::vector< Common::ItemSlotEx >& bankSlots)// for snapshot
			{
				AutoLock lock(cs);

				//std::vector<Common::ItemSlotEx> equipSlots;
				//std::vector<Common::ItemSlotEx> inventorySlots;
				//std::vector<Common::ItemSlotEx> bankSlots;

				equip.AllItems(equipSlots);
				inventory.AllItems(inventorySlots);
				bank.AllItems(bankSlots);

				//origine�� ���Ͽ� üũ �Ѵ�.
				ChangedEquipSlotFilter(equipSlots, changedItems);
				ChangedInventorySlotFilter(inventorySlots, changedItems);
				ChangedBankSlotFilter(bankSlots, changedItems);

				////���� �����ϴ� ������ �����۵��� allItems�� �ִ´�(snapshot��).
				//allItems.clear();
				//allItems.insert(allItems.begin(),  equipSlots.begin(), equipSlots.end() );
				//allItems.insert(allItems.end(),  inventorySlots.begin(), inventorySlots.end() );
				//allItems.insert(allItems.end(),  bankSlots.begin(), bankSlots.end() );

				//back up item data
				originSlots.Clear();
				originSlots.Equip = equipSlots;
				originSlots.Bank = bankSlots;
				originSlots.Inventory = inventorySlots;
				petsInventory.AllChangedItemsAndOriginBackup(petsChangedItems, petsAllItems);
			}

			void Items::RebirthChangedItemsAndOriginBackup(ChangedItems& changedItems, std::vector< Common::ItemSlotEx >& inventorySlots)
			{
				AutoLock lock(cs);

				inventory.AllItems(inventorySlots);

				//origine�� ���Ͽ� üũ �Ѵ�.
				ChangedInventorySlotFilter(inventorySlots, changedItems);

				//back up item data
				originSlots.Inventory = inventorySlots;
			}

			void Items::RebirthBackUp()
			{
				AutoLock lock(cs);
				rebirthRollBackInventory = originSlots.Inventory;
			}

			void Items::RebirthRollBack()
			{
				AutoLock lock(cs);
				originSlots.Inventory = rebirthRollBackInventory;
			}

			bool Items::IsExpiredItem()
			{
				std::vector< Common::ItemSlotEx > InventoryItems;
				inventory.AllItems(InventoryItems);
				bool expired = false;

				for (std::vector< Common::ItemSlotEx >::iterator i = InventoryItems.begin(), end = InventoryItems.end(); i != end; ++i)
				{
					const Common::ItemEx* item = GetItem(i->Position.Bag, i->Position.Position, i->IsBank);
					if (item == NULL) continue;
					if (Database::Enchant::IsExpired(item->Info, item->InstanceEx) && !(item->Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::EXISTWHENEXPIRED))
					{
						expired = true;
						DropItems(i->Position.Bag, i->Position.Position, i->Stacked, true);
						continue;
					}
				}
				return expired;
			}

			void Items::BankItemList(std::vector< XRated::ItemSlot >& listBankItem)
			{
				bank.AllItems(listBankItem);
			}


			SlotPos Items::GetFirstBlankSlot()
			{
				ItemPosition pos = inventory.FindEmptySlot(NULL);
				if (pos == ItemPosition::Invalid)
					return SlotPos(-1, -1);
				return SlotPos(pos.Bag, pos.Position);
			}


			void Items::SenditemRequirementError()
			{
				Protocol::FromServer::Error error;
				error.errorcode = Errors::Unexpected;
				error.errorstring = L"������ ó���� �Դϴ�.";
				Send(error);
			}

			void Items::Send(Protocol::IPacketSerializable& packet)
			{
				event.ItemsSend(packet);
			}

			void Items::SendToAll(Protocol::IPacketSerializable& packet)
			{
				event.ItemsSendToAll(packet);
			}

			void Items::Log(const char* operation, const Common::ItemEx& item)
			{
				if (item.Info == NULL) return;

				if (IsImportantToLog(item.Info))
					;
					/*Logger().Write(IAdmLogger::ImportantLogger, operation, event.GetName(), Service::Http::Logger::Parameter()
						<< item.Info->Hash << item.Serial << item.StackedCount << item.InstanceEx.Instance << item.InstanceEx.ExpireDate.ToString());*/
			}

			//void Items::ChangedSlotFilter(const std::vector< Common::ItemEx >& origin, const std::vector< Common::ItemEx >& now,ChangedItems& in, bool isBank,uint8 bag)
			//{
			//	size_t max = now.size();
			//	Common::ItemSlotEx itemSlot;
			//	itemSlot.IsBank = isBank?1:0;
			//	itemSlot.Position.Bag = bag;
			//	
			//	for( size_t i =0; i < max; ++i )
			//	{
			//		const Common::ItemEx& originSlot(origin[i]);
			//		const Common::ItemEx& nowSlot(now[i]);
			//		if( (originSlot != nowSlot) && ((originSlot.Info != NULL) || (nowSlot.Info != NULL)) ) {
			//			
			//			if( nowSlot.Info == NULL ) {
			//				//drop���� üũ.					
			//				itemSlot.ItemInfo = originSlot.Info;					
			//				itemSlot.Id = originSlot.Info->Hash;
			//				itemSlot.serial = originSlot.Serial;					
			//				itemSlot.Position.Position = i;
			//				itemSlot.Stacked = originSlot.StackedCount;
			//				itemSlot.Instance = originSlot.Instance;
			//				

			//				in.Delete.push_back(itemSlot);
			//			}else if( originSlot.Info == NULL ) {
			//				//insert���� üũ.					
			//				itemSlot.ItemInfo = nowSlot.Info;					
			//				itemSlot.Id = nowSlot.Info->Hash;
			//				itemSlot.serial = nowSlot.Serial;					
			//				itemSlot.Position.Position = i;
			//				itemSlot.Stacked = nowSlot.StackedCount;
			//				itemSlot.Instance = nowSlot.Instance;					

			//				in.Insert.push_back(itemSlot);
			//			}else {
			//				//update���� üũ.					
			//				itemSlot.ItemInfo = nowSlot.Info;					
			//				itemSlot.Id = nowSlot.Info->Hash;
			//				itemSlot.serial = nowSlot.Serial;					
			//				itemSlot.Position.Position = i;
			//				itemSlot.Stacked = nowSlot.StackedCount;
			//				itemSlot.Instance = nowSlot.Instance;					

			//				in.Update.push_back(itemSlot);
			//			}
			//		}
			//	}
			//}

			void Items::ChangedEquipSlotFilter(std::vector< Common::ItemSlotEx >& nowEquip, ChangedItems& in)
			{
				std::vector< Common::ItemSlotEx >::iterator originIter;
				std::vector< Common::ItemSlotEx >::iterator newIter;

				std::vector< uint8 > bags;
				equip.GetEquipBags(bags);
				for (size_t index = 0; index < bags.size(); ++index)
				{
					for (int i = 0; i < XRated::Constants::Equipment::Cnt; ++i)
					{
						originIter = std::find_if(originSlots.Equip.begin(), originSlots.Equip.end(), Common::ItemSlotEx::FinderByPos(XRated::ItemPosition(static_cast<uint8>(bags[index]), static_cast<uint8>(i))));
						newIter = std::find_if(nowEquip.begin(), nowEquip.end(), Common::ItemSlotEx::FinderByPos(XRated::ItemPosition(static_cast<uint8>(bags[index]), static_cast<uint8>(i))));

						if (originIter != originSlots.Equip.end() && newIter == nowEquip.end())
						{
							in.Delete.push_back(*originIter);
						}
						else if (originIter == originSlots.Equip.end() && newIter != nowEquip.end())
						{
							in.Insert.push_back(*newIter);
						}
						else  if (originIter != originSlots.Equip.end() && newIter != nowEquip.end() && (*originIter) != (*newIter))
						{
							in.Update.push_back(*newIter);
						}
					}
				}
			}

			void Items::ChangedInventorySlotFilter(std::vector< Common::ItemSlotEx >& nowInventory, ChangedItems& in)
			{
				std::vector< uint8 > bags;
				inventory.GetBags(bags);
				for (size_t index = 0; index < bags.size(); ++index)
				{
					for (int j = 0; j < XRated::Constants::MaxOfSlotsInInventoryBag; ++j)
					{
						std::vector< Common::ItemSlotEx >::iterator originIter = std::find_if(originSlots.Inventory.begin(), originSlots.Inventory.end(), Common::ItemSlotEx::FinderByPos(XRated::ItemPosition(static_cast<uint8>(bags[index]), static_cast<uint8>(j))));
						std::vector< Common::ItemSlotEx >::iterator newIter = std::find_if(nowInventory.begin(), nowInventory.end(), Common::ItemSlotEx::FinderByPos(XRated::ItemPosition(static_cast<uint8>(bags[index]), static_cast<uint8>(j))));

						if (originIter != originSlots.Inventory.end() && newIter == nowInventory.end()) {
							in.Delete.push_back(*originIter);
							continue;
						}

						if (originIter == originSlots.Inventory.end() && newIter != nowInventory.end()) {
							in.Insert.push_back(*newIter);
							continue;
						}

						if (originIter != originSlots.Inventory.end() && newIter != nowInventory.end() && (*originIter) != (*newIter)) {
							in.Update.push_back(*newIter);
							continue;
						}
					}
				}
			}

			void Items::ChangedBankSlotFilter(std::vector< Common::ItemSlotEx >& nowBank, ChangedItems& in)
			{
				for (int i = 0; i < XRated::Constants::MaxOfBankBags; ++i)
				{
					for (int j = 0; j < XRated::Constants::MaxOfSlotsInBankBag; ++j)
					{
						std::vector< Common::ItemSlotEx >::iterator originIter = std::find_if(originSlots.Bank.begin(), originSlots.Bank.end(), Common::ItemSlotEx::FinderByPos(XRated::ItemPosition(static_cast<uint8>(i), static_cast<uint8>(j))));
						std::vector< Common::ItemSlotEx >::iterator newIter = std::find_if(nowBank.begin(), nowBank.end(), Common::ItemSlotEx::FinderByPos(XRated::ItemPosition(static_cast<uint8>(i), static_cast<uint8>(j))));

						if (originIter != originSlots.Bank.end() && newIter == nowBank.end()) {
							in.Delete.push_back(*originIter);
							continue;
						}

						if (originIter == originSlots.Bank.end() && newIter != nowBank.end()) {
							in.Insert.push_back(*newIter);
							continue;
						}


						if (originIter != originSlots.Bank.end() && newIter != nowBank.end() && (*originIter) != (*newIter)) {
							in.Update.push_back(*newIter);
							continue;
						}
					}
				}
			}


			//Items::AllItemSlots::AllItemSlots() :Equip(XRated::Constants::Equipment::Cnt, Common::ItemEx()) {
			//	Inventory.reserve(XRated::Constants::MaxOfInventoryBags);
			//	size_t max = XRated::Constants::MaxOfInventoryBags;
			//	for(size_t i=0;i < max; ++i )
			//	{
			//		Inventory.push_back(std::vector< Common::ItemEx >(XRated::Constants::MaxOfSlotsInInventoryBag, Common::ItemEx()));
			//	}

			//	Bank.reserve(XRated::Constants::MaxOfBankBags);
			//	max =XRated::Constants::MaxOfBankBags;
			//	for(i=0;i < max; ++i )
			//	{
			//		Bank.push_back(std::vector< Common::ItemEx >(XRated::Constants::MaxOfSlotsInBankBag, Common::ItemEx()));
			//	}
			//	Clear();
			//}	

			//void Items::AllItemSlots::SetBankItem(uint32 bag, uint32 pos, const Common::ItemEx& item)
			//{
			//	if(bag >= Bank.size())
			//	{
			//		ALLM_ERROR(( L"Inventory::SetBankItem() - bag({}) > Bank.size()", bag ));
			//		return;
			//	}

			//	std::vector< Common::ItemEx >& items( Bank[bag] );
			//	if (pos>=items.size()) {
			//		// out of range
			//		ALLM_ERROR(( L"Inventory::SetBankItem() - position({}) > items size", pos ));
			//		return;
			//	}
			//	items[pos]=item;
			//}

			//void Items::AllItemSlots::SetInventoryItem(uint32 bag, uint32 pos, const Common::ItemEx& item)
			//{
			//	if(bag >= Inventory.size())
			//	{
			//		ALLM_ERROR(( L"Inventory::SetInventoryItem() - bag({}) > Inventory.size()", bag ));
			//		return;
			//	}

			//	std::vector< Common::ItemEx >& items( Inventory[bag] );
			//	if (pos>=items.size()) {
			//		// out of range
			//		ALLM_ERROR(( L"Inventory::SetInventoryItem() - position({}) > items size", pos ));
			//		return;
			//	}
			//	items[pos]=item;
			//			
			//}

			//void Items::AllItemSlots::SetEquip(uint32 pos, const Common::ItemEx& item)
			//{
			//	if( pos >= Equip.size() )
			//	{
			//		ALLM_ERROR(( L"Equip::PushItem() - Error : pos({}) >= Max({})", pos, Equip.size() ));
			//		return;
			//	}
			//	Equip[pos] = item;
			//}

			//void Items::AllItemSlots::Clear() {
			//	size_t max = Equip.size();
			//	for(size_t i = 0 ; i < max ; ++i)
			//		Equip[i].Info=NULL;

			//	max = Inventory.size();
			//	for(size_t i=0;i < max; ++i )
			//	{
			//		std::vector< Common::ItemEx >::iterator iter = Inventory[i].begin();
			//		const std::vector< Common::ItemEx >::iterator end = Inventory[i].end();
			//		while( iter != end ) {
			//			(*iter).Info = NULL;
			//			++iter;
			//		}
			//	}

			//	max = Bank.size();
			//	for(i=0;i < max; ++i )
			//	{
			//		std::vector< Common::ItemEx >::iterator iter = Bank[i].begin();
			//		const std::vector< Common::ItemEx >::iterator end = Bank[i].end();
			//		while( iter != end ) {
			//			(*iter).Info = NULL;
			//			++iter;
			//		}
			//	}

			//	
			//}


			Items::AllItemSlots::AllItemSlots()
			{
				Clear();
			}

			void Items::AllItemSlots::Clear()
			{
				Equip.clear();
				Inventory.clear();
				Bank.clear();
			}

			void Items::AllItemSlots::SetBankItem(uint32 bag, uint32 pos, const Common::ItemEx& item)
			{
				if (item.Info == NULL) return;

				Common::ItemSlotEx itemSlotEx;
				itemSlotEx.ItemInfo = item.Info;
				itemSlotEx.Id = item.Info->Hash;
				itemSlotEx.serial = item.Serial;
				itemSlotEx.Stacked = item.StackedCount;
				itemSlotEx.Position.Bag = static_cast<uint8>(bag);
				itemSlotEx.Position.Position = static_cast<uint8>(pos);
				itemSlotEx.IsBank = 1;
				itemSlotEx.InstanceEx = item.InstanceEx;

				Bank.push_back(itemSlotEx);
			}

			void Items::AllItemSlots::SetInventoryItem(uint32 bag, uint32 pos, const Common::ItemEx& item)
			{
				if (item.Info == NULL) return;

				Common::ItemSlotEx itemSlotEx;
				itemSlotEx.ItemInfo = item.Info;
				itemSlotEx.Id = item.Info->Hash;
				itemSlotEx.serial = item.Serial;
				itemSlotEx.Stacked = item.StackedCount;
				itemSlotEx.Position.Bag = static_cast<uint8>(bag);
				itemSlotEx.Position.Position = static_cast<uint8>(pos);
				itemSlotEx.IsBank = 0;
				itemSlotEx.InstanceEx = item.InstanceEx;

				Inventory.push_back(itemSlotEx);
			}

			void Items::AllItemSlots::SetEquip(uint8 bag, uint32 pos, const Common::ItemEx& item)
			{
				if (item.Info == NULL) return;

				Common::ItemSlotEx itemSlotEx;
				itemSlotEx.ItemInfo = item.Info;
				itemSlotEx.Id = item.Info->Hash;
				itemSlotEx.serial = item.Serial;
				itemSlotEx.Stacked = item.StackedCount;
				itemSlotEx.Position.Bag = bag;
				itemSlotEx.Position.Position = static_cast<uint8>(pos);
				itemSlotEx.IsBank = 0;
				itemSlotEx.InstanceEx = item.InstanceEx;

				Equip.push_back(itemSlotEx);
			}


		}
	}
} 