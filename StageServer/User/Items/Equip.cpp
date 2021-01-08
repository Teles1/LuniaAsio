
#include "Equip.h"
#include <Info/Info.h>


namespace Lunia {
	namespace XRated {
		namespace StageServer {


			Equip::Equip()
				: currentEquipSet(0)
				, currentCashEquipSet(0)
			{
				Clear();
			}

			Equip::~Equip(void)
			{
			}

			void Equip::Clear()
			{
				equipSet.clear();
				cashEquipSet.clear();
				etcEquips.clear();

				uint8 set = currentEquipSet;
				EquipSet::iterator it = equipSet.find(set);
				while (it == equipSet.end())
				{
					equipSet.insert(std::make_pair(set, Equipments()));
					for (int index = 0; index < XRated::Constants::Equipment::CashBody; ++index)
					{
						equipSet[set][static_cast<XRated::Constants::Equipment>(index)] = Common::ItemEx();
					}
					set = Database::DatabaseInstance().InfoCollections.BagInfos.SetIndexToBagNumber(
						Database::DatabaseInstance().InfoCollections.BagInfos.GetNextEquipIndex(set));
					it = equipSet.find(set);
				}

				set = currentCashEquipSet;
				it = cashEquipSet.find(set);
				while (it == cashEquipSet.end())
				{
					cashEquipSet.insert(std::make_pair(set, Equipments()));
					for (int index = XRated::Constants::Equipment::CashBody; index <= XRated::Constants::Equipment::CashEtc2; ++index)
					{
						cashEquipSet[set][static_cast<XRated::Constants::Equipment>(index)] = Common::ItemEx();
					}
					set = Database::DatabaseInstance().InfoCollections.BagInfos.SetCashIndexToBagNumber(
						Database::DatabaseInstance().InfoCollections.BagInfos.GetNextCashEquipIndex(set));
					it = cashEquipSet.find(set);
				}
			}

			bool Equip::IsCashEquipment(XRated::Constants::Equipment position) const
			{
				return (position >= XRated::Constants::Equipment::CashBody && position <= XRated::Constants::Equipment::CashEtc2);
			}

			bool Equip::IsEquipment(XRated::Constants::Equipment position) const
			{
				return (position < XRated::Constants::Equipment::CashBody);
			}

			bool Equip::IsEtcEquipments(XRated::Constants::Equipment position) const
			{
				return (position >= XRated::Constants::Equipment::Pet && position <= XRated::Constants::Equipment::FameNoun);
			}

			bool Equip::SetItems(uint8 set, uint32 pos, const Common::ItemEx& item)
			{
				XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
				if (IsCashEquipment(position) == true)
				{
					cashEquipSet[set][position] = item;
				}
				else if (IsEquipment(position) == true)
				{
					equipSet[set][position] = item;
				}
				else if (IsEtcEquipments(position) == true)
				{
					etcEquips[position] = item;
				}
				else
				{
					LoggerInstance().Error(L"Equip::SetItems - out of boundary error : {0}", pos);
					return false;
				}

				return true;
			}

			bool Equip::PushItems(uint32 pos, const Common::ItemEx& item)
			{
				XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
				if (IsCashEquipment(position) == true)
				{
					cashEquipSet[currentCashEquipSet][position] = item;
				}
				else if (IsEquipment(position) == true)
				{
					equipSet[currentEquipSet][position] = item;
				}
				else if (IsEtcEquipments(position) == true)
				{
					etcEquips[position] = item;
				}
				else
				{
					LoggerInstance().Error(L"Equip::PushItems - out of boundary error : {0}", pos);
					return false;
				}

				return true;
			}


			void Equip::DeleteItem(uint32 pos)
			{
				XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
				if (IsCashEquipment(position) == true)
				{
					cashEquipSet[currentCashEquipSet][position].Info = NULL;
				}
				else if (IsEquipment(position) == true)
				{
					equipSet[currentEquipSet][position].Info = NULL;
				}
				else if (IsEtcEquipments(position) == true)
				{
					etcEquips[position].Info = NULL;
				}
				else
				{
					LoggerInstance().Error(L"Equip::DeleteItem - out of boundary error : {0}", pos);
				}
			}

			void Equip::MoveItems(uint32 fromPos, uint32 toPos)
			{
				XRated::Constants::Equipment fromPosition = static_cast<XRated::Constants::Equipment>(fromPos);
				XRated::Constants::Equipment toPosition = static_cast<XRated::Constants::Equipment>(toPos);
				Equipments::iterator fromEquip, toEquip;
				bool isFromValid = false;
				bool isToValid = false;

				if (IsCashEquipment(fromPosition) == true)
				{
					fromEquip = cashEquipSet[currentCashEquipSet].find(fromPosition);
					isFromValid = true;
				}
				else if (IsEquipment(fromPosition) == true)
				{
					fromEquip = equipSet[currentEquipSet].find(fromPosition);
					isFromValid = true;
				}
				else if (IsEtcEquipments(fromPosition) == true)
				{
					fromEquip = etcEquips.find(fromPosition);
					isFromValid = true;
				}

				if (IsCashEquipment(toPosition) == true)
				{
					toEquip = cashEquipSet[currentCashEquipSet].find(toPosition);
					isToValid = true;
				}
				else if (IsEquipment(toPosition) == true)
				{
					toEquip = equipSet[currentEquipSet].find(toPosition);
					isToValid = true;
				}
				else if (IsEtcEquipments(fromPosition) == true)
				{
					toEquip = etcEquips.find(toPosition);
					isToValid = true;
				}

				if (isFromValid && isToValid)
				{
					const Common::ItemEx temp = fromEquip->second;
					fromEquip->second = toEquip->second;
					toEquip->second = temp;
				}
				else
				{
					LoggerInstance().Error("Equip::MoveItems - out of boundary error : {0} => {1}", fromPos, toPos);
				}
			}

			bool Equip::IsCurrentEquipSet(uint8 set, uint32 pos)
			{
				XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
				if (IsEquipment(position) == true)
				{
					return set == currentEquipSet;
				}
				else if (IsCashEquipment(position) == true)
				{
					return set == currentCashEquipSet;
				}
				else if (IsEtcEquipments(position) == true)
				{
					return true;
				}

				return false;
			}

			bool Equip::ChangeEquipSet(uint8 set)
			{
				if (equipSet.find(set) == equipSet.end())
				{
					return false;
				}

				currentEquipSet = set;
				return true;
			}

			bool Equip::ChangeCashEquipSet(uint8 set)
			{
				if (cashEquipSet.find(set) == cashEquipSet.end())
				{
					return false;
				}
				currentCashEquipSet = set;
				return true;
			}

			uint8 Equip::GetCurrentEquipSet()
			{
				return currentEquipSet;
			}

			uint8 Equip::GetCurrentCashEquipSet()
			{
				return currentCashEquipSet;
			}

			void Equip::GetEquipBags(std::vector<uint8>& bags)
			{
				bags.clear();

				EquipSet::iterator it = equipSet.begin();
				for (; it != equipSet.end(); ++it)
				{
					std::vector<uint8>::const_iterator iter = std::find(bags.begin(), bags.end(), it->first);
					if (iter == bags.end())
						bags.push_back(it->first);
				}

				it = cashEquipSet.begin();
				for (; it != cashEquipSet.end(); ++it)
				{
					std::vector<uint8>::const_iterator iter = std::find(bags.begin(), bags.end(), it->first);
					if (iter == bags.end())
						bags.push_back(it->first);
				}
			}

			const Common::ItemEx* Equip::GetItem(uint32 pos) const
			{
				XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
				const Common::ItemEx* item = NULL;

				if (IsCashEquipment(position) == true)
				{
					EquipSet::const_iterator it = cashEquipSet.find(currentCashEquipSet);
					if (it != cashEquipSet.end())
					{
						Equipments::const_iterator equip = it->second.find(position);
						if (equip != it->second.end())
						{
							item = &(equip->second);
						}
					}
				}
				else if (IsEquipment(position) == true)
				{
					EquipSet::const_iterator it = equipSet.find(currentEquipSet);
					if (it != equipSet.end())
					{
						Equipments::const_iterator equip = it->second.find(position);
						if (equip != it->second.end())
						{
							item = &(equip->second);
						}
					}
				}
				else if (IsEtcEquipments(position) == true)
				{
					Equipments::const_iterator equip = etcEquips.find(position);
					if (equip != etcEquips.end())
					{
						item = &(equip->second);
					}
				}

				if (item != NULL && item->Info != NULL && item->StackedCount > 0)
				{
					return item;
				}

				return NULL;
			}

			void Equip::GetNormalEquipments(std::vector< XRated::EquippedItem >& list)
			{
				list.clear();
				for (int pos = 0; pos < XRated::Constants::Equipment::Cnt; ++pos)
				{
					XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
					if (IsEquipment(position) == true)
					{
						XRated::EquippedItem item;
						item.where = position;

						if (equipSet[currentEquipSet][position].Info != NULL)
						{
							item.itemHash = equipSet[currentEquipSet][position].Info->Hash;
							item.InstanceEx = equipSet[currentEquipSet][position].InstanceEx;
						}
						else
						{
							item.itemHash = 0;
							item.InstanceEx = 0;
						}
						list.push_back(item);
					}
				}
			}

			void Equip::GetCashEquipments(std::vector< XRated::EquippedItem >& list)
			{
				list.clear();
				for (int pos = 0; pos < XRated::Constants::Equipment::Cnt; ++pos)
				{
					XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
					if (IsCashEquipment(position) == true)
					{
						XRated::EquippedItem item;
						item.where = position;

						if (cashEquipSet[currentCashEquipSet][position].Info != NULL)
						{
							item.itemHash = cashEquipSet[currentCashEquipSet][position].Info->Hash;
							item.InstanceEx = cashEquipSet[currentCashEquipSet][position].InstanceEx;
						}
						else
						{
							item.itemHash = 0;
							item.InstanceEx = 0;
						}
						list.push_back(item);
					}
				}
			}

			bool Equip::SetItemInstance(uint32 pos, InstanceEx instance)
			{
				XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
				if (IsCashEquipment(position) == true)
				{
					Common::ItemEx& item = cashEquipSet[currentCashEquipSet][position];
					if (item.Info != NULL && item.StackedCount > 0)
					{
						item.InstanceEx = instance;
						return true;
					}
				}
				else if (IsEquipment(position) == true)
				{
					Common::ItemEx& item = equipSet[currentEquipSet][position];
					if (item.Info != NULL && item.StackedCount > 0)
					{
						item.InstanceEx = instance;
						return true;
					}
				}
				else if (IsEtcEquipments(position) == true)
				{
					Common::ItemEx& item = etcEquips[position];
					if (item.Info != NULL && item.StackedCount > 0)
					{
						item.InstanceEx = instance;
						return true;
					}
				}

				LoggerInstance().Error("Equip::GetItem() - Error : pos({0})", pos);
				return false;
			}

			void Equip::GetEquip(std::vector<Common::ItemEx>& items) const
			{
				items.clear();
				for (int pos = 0; pos < XRated::Constants::Equipment::Cnt; ++pos)
				{
					XRated::Constants::Equipment position = static_cast<XRated::Constants::Equipment>(pos);
					if (IsCashEquipment(position) == true)
					{
						EquipSet::const_iterator it = cashEquipSet.find(currentCashEquipSet);
						if (it != cashEquipSet.end())
						{
							Equipments::const_iterator equip = it->second.find(position);
							if (equip != it->second.end())
							{
								items.push_back(equip->second);
								continue;
							}
						}
					}
					else if (IsEquipment(position) == true)
					{
						EquipSet::const_iterator it = equipSet.find(currentEquipSet);
						if (it != equipSet.end())
						{
							Equipments::const_iterator equip = it->second.find(position);
							if (equip != it->second.end())
							{
								items.push_back(equip->second);
								continue;
							}
						}
					}
					else if (IsEtcEquipments(position) == true)
					{
						Equipments::const_iterator equip = etcEquips.find(position);
						if (equip != etcEquips.end())
						{
							items.push_back(equip->second);
							continue;
						}
					}
					items.push_back(Common::ItemEx());
				}
			}

			void Equip::AllItems(std::vector< Common::ItemSlotEx >& allItems)
			{
				EquipSet::iterator it = equipSet.begin();
				for (; it != equipSet.end(); ++it)
				{
					Equipments::iterator itemIter = it->second.begin();
					for (; itemIter != it->second.end(); ++itemIter)
					{
						if (itemIter->second.Info == NULL)
							continue;

						Common::ItemSlotEx itemSlot;
						itemSlot.ItemInfo = itemIter->second.Info;
						itemSlot.IsBank = false;
						itemSlot.Id = itemIter->second.Info->Hash;
						itemSlot.serial = itemIter->second.Serial;
						itemSlot.Position.Bag = it->first;
						itemSlot.Position.Position = itemIter->first;
						itemSlot.Stacked = itemIter->second.StackedCount;
						itemSlot.InstanceEx = itemIter->second.InstanceEx;

						allItems.push_back(itemSlot);
					}
				}

				it = cashEquipSet.begin();
				for (; it != cashEquipSet.end(); ++it)
				{
					Equipments::iterator itemIter = it->second.begin();
					for (; itemIter != it->second.end(); ++itemIter)
					{
						if (itemIter->second.Info == NULL)
							continue;

						Common::ItemSlotEx itemSlot;
						itemSlot.ItemInfo = itemIter->second.Info;
						itemSlot.IsBank = false;
						itemSlot.Id = itemIter->second.Info->Hash;
						itemSlot.serial = itemIter->second.Serial;
						itemSlot.Position.Bag = it->first;
						itemSlot.Position.Position = itemIter->first;
						itemSlot.Stacked = itemIter->second.StackedCount;
						itemSlot.InstanceEx = itemIter->second.InstanceEx;

						allItems.push_back(itemSlot);
					}
				}

				{
					Equipments::iterator itemIter = etcEquips.begin();
					for (; itemIter != etcEquips.end(); ++itemIter)
					{
						if (itemIter->second.Info == NULL)
							continue;

						Common::ItemSlotEx itemSlot;
						itemSlot.ItemInfo = itemIter->second.Info;
						itemSlot.IsBank = false;
						itemSlot.Id = itemIter->second.Info->Hash;
						itemSlot.serial = itemIter->second.Serial;
						itemSlot.Position.Bag = 0;	//etcItems�� ������ 0�������̴�.
						itemSlot.Position.Position = itemIter->first;
						itemSlot.Stacked = itemIter->second.StackedCount;
						itemSlot.InstanceEx = itemIter->second.InstanceEx;

						allItems.push_back(itemSlot);
					}
				}
			}

			XRated::ItemPosition Equip::FindItem(XRated::GlobalSerial itemSerial) const
			{
				EquipSet::const_iterator it = equipSet.begin();
				for (; it != equipSet.end(); ++it)
				{
					Equipments::const_iterator itemIter = it->second.begin();
					for (; itemIter != it->second.end(); ++itemIter)
					{
						if (itemIter->second.Serial == itemSerial)
						{
							return XRated::ItemPosition(0, itemIter->first);
						}
					}
				}

				it = cashEquipSet.begin();
				for (; it != cashEquipSet.end(); ++it)
				{
					Equipments::const_iterator itemIter = it->second.begin();
					for (; itemIter != it->second.end(); ++itemIter)
					{
						if (itemIter->second.Serial == itemSerial)
						{
							return XRated::ItemPosition(0, itemIter->first);
						}
					}
				}

				{
					Equipments::const_iterator itemIter = etcEquips.begin();
					for (; itemIter != etcEquips.end(); ++itemIter)
					{
						if (itemIter->second.Serial == itemSerial)
						{
							return XRated::ItemPosition(0, itemIter->first);
						}
					}
				}

				return XRated::ItemPosition::Invalid;
			}


		}
	}
}