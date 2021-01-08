#include "PetInventory.h"
#include <StageServer/User/User.h>
#include <StageServer/User/IUserRoom.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {


			///////////// PetItem Visitor ////////////////////////
			class PetInfoRetrieveVisitor
				: public IPetItemVisitor
			{
			private:
				const XRated::Pet* pet;
				XRated::GlobalSerial petSerial;

			public:
				PetInfoRetrieveVisitor(XRated::GlobalSerial petSerial)
					: pet(NULL), petSerial(petSerial)
				{
				}

				const XRated::Pet* GetPetInfo() const { return pet; }

				bool Run(User* user, IUserRoom* /*room*/)
				{
					if (user == NULL)
						return false;

					if (user->GetPlayer())
					{
						for (size_t i = 0; i < user->GetPlayer()->GetPlayerPetData().size(); ++i)
						{
							if (user->GetPlayer()->GetPlayerPetData()[i].Pet.PetSerial == petSerial)
							{
								pet = &user->GetPlayer()->GetPlayerPetData()[i].Pet;

								break;
							}
						}
					}
					else {
						AutoLock cs(user->GetPetDatas().GetSyncObject());
						pet = user->GetPetDatas().GetPetData(petSerial);
					}

					return true;
				}
			};

			///////////// PetItem Visitor ////////////////////////

			void PetsInventory::ChangedItems::Clear()
			{
				Insert.clear();
				Update.clear();
				Delete.clear();
			}

			bool PetsInventory::Inventory::IsValidPosition(XRated::PetItemSlot::PositionType type, uint8 position)
			{
				switch (type)
				{
				case	XRated::PetItemSlot::PositionType::Equipment:
				{
					int pos = (static_cast<int>(position) - static_cast<int>(Constants::Equipment::Pet_Mask));

					if (pos < 0 || pos >= XRated::Constants::Equipment::MaxPetEquipCnt)
						return false;
				}
				break;
				case	XRated::PetItemSlot::PositionType::Inventory:
				{
					if (position >= inventorySize)
						return false;
				}
				break;
				}

				return true;
			}
			bool PetsInventory::Inventory::IsHaveItem() const
			{
				ItemSlots::const_iterator iter = Equip.begin();
				ItemSlots::const_iterator end = Equip.end();
				while (iter != end) {
					if ((*iter).Info != NULL) {
						return true;
					}
					++iter;
				}

				iter = Inven.begin();
				end = Inven.end();
				while (iter != end) {
					if ((*iter).Info != NULL) {
						return true;
					}
					++iter;
				}
				return false;
			}

			PetsInventory::Inventory::Inventory(const XRated::GlobalSerial& pet, int InventorySize)
				: PetSerial(pet), Equip(XRated::Constants::Equipment::MaxPetEquipCnt), inventorySize(InventorySize), IsChecked(false)
			{
				Inven.resize(inventorySize);
			}

			PetsInventory::Inventory::~Inventory()
			{
			}

			const XRated::GlobalSerial& PetsInventory::Inventory::GetPetSerial() const
			{
				return PetSerial;
			}



			PetsInventory::PetsInventory(IPetItemVisitorListener* userVisitor)
				: userVisitor(userVisitor)
			{
			}

			PetsInventory::~PetsInventory()
			{
			}

			void PetsInventory::Init()
			{
				originInventoryList.clear();
				inventoryList.clear();
			}
			int PetsInventory::SetItemList(const std::string& items, std::vector< XRated::Pet >& pets, Protocol::FromServer::ListPetItem& packet)
			{
				/*
				AllM::XRated::Service::Http::TextPacket::Subpacket parser;
				uint16 i(0);

				int expired = 0;
				AutoLock lock(itemsCS);

				SetPetsInventorySize(pets);

				//init
				if (items.empty()) // empty items
					return 0;


				std::vector<PetItemSlot>	itemsTemp;
				parser.Parse(items.c_str());
				i = 0;
				std::set<GlobalSerial> serials; // buffer to check item duplication
				while (i < parser.GetCount())
				{
					//petItemSerial, Position Type, position, itemHash, itemSerial, instance, expireDate
					Common::ItemEx	item;

					GlobalSerial petSerial = StringUtil::To<uint64>(parser[i++]);
					XRated::PetItemSlot::PositionType type = static_cast<XRated::PetItemSlot::PositionType>(StringUtil::To<int>(parser[i++]));
					uint8 position = static_cast<uint8>(StringUtil::To<int>(parser[i++]));
					uint32 id = StringUtil::To<uint32>(parser[i++]); // item hash
					item.StackedCount = static_cast<uint16>(StringUtil::To<int>(parser[i++]));
					item.Serial = StringUtil::To<uint64>(parser[i++]);
					item.InstanceEx = StringUtil::To<int64>(parser[i++]);
					item.InstanceEx.ExpireDate.Parse(StringUtil::ToUnicode(parser[i++]));


					item.Info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(id);
					if (item.Info == NULL || item.StackedCount == 0)
					{
						ALLM_WARNING((L"invalid item information (hash:%d, count:%d)", id, item.StackedCount));
						continue;
					}

#ifdef BAN_DUPLICATED_ITEM_SERIAL
					// check duplication
					if (item.Info->MaxCnt == 1 && item.Serial) // items which serial should not be changed and is not from manual(administration or so)
					{
						if (serials.insert(item.Serial).second == false) // the set already contained an element
						{
							ALLM_ERROR((L"duplicated item(%u) detected(%I64u) on a pet(%I64u)", item.Info->Hash, item.Serial, petSerial));
							return -1;
						}
					}
#endif
					//back up origin
					SetItemWithNoLock(originInventoryList, petSerial, type, position, item);

					if (item.Info->Attributes.Attr & (1 << 8))
						continue;

					// check expiration
					if (Database::Enchant::IsExpired(item.Info, item.InstanceEx) && !(item.Info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::EXISTWHENEXPIRED))
					{
						//TODO : expiredItem log�� ���ܾ� �Ѵ�.
						//Log("expireditem", item);
						++expired;
						continue;
					}

					//clipping stack count - actually this should not be happended.
					if (item.StackedCount > static_cast<uint32>(item.Info->MaxCnt))
					{
						ALLM_IMPORTANT((L"Items::SetItemList() item.Id(%u) count(%u) > pItemInfo->MaxCnt(%u)", id, item.StackedCount, item.Info->MaxCnt));
						item.StackedCount = static_cast<uint16>(item.Info->MaxCnt);
					}
					//add pet inventory
					SetItemWithNoLock(petSerial, type, position, item);
					//pet equipment
					if (type == XRated::PetItemSlot::Equipment) {
						std::vector<XRated::Pet>::iterator iter = std::find_if(pets.begin(), pets.end(), XRated::Pet::Finder(petSerial));
						if (iter != pets.end()) {
							(*iter).Equipment(id, item.InstanceEx, static_cast<XRated::Constants::Equipment>(position));
						}
					}

					//packet
					packet.PetsItems[petSerial].push_back(PetItemSlot(type, position, id, item.InstanceEx, item.StackedCount));
				}
				return expired;
				*/
				return 0;
			}

			void PetsInventory::AddInventory(const XRated::Pet& pet)
			{
				const auto species = XRated::Database::DatabaseInstance().InfoCollections.Pets.GetSpecies(pet.PetHash);

				if (!species)
				{
					SetInventorySize(inventoryList, pet.PetSerial, *species);
					SetInventorySize(originInventoryList, pet.PetSerial, *species);
				}
				else
					LoggerInstance().Warn("can not found species {0}", pet.PetHash);
			}
			void PetsInventory::SetPetsInventorySize(const std::vector<XRated::Pet>& pets)
			{
				std::vector<XRated::Pet>::const_iterator iter = pets.begin();
				std::vector<XRated::Pet>::const_iterator end = pets.end();

				while (iter != end) {

					const XRated::Database::Info::Pet::Species* species = XRated::Database::DatabaseInstance().InfoCollections.Pets.GetSpecies((*iter).PetHash);
					if (species != NULL) {
						//inventory�� size�� �����Ѵ�.
						SetInventorySize(inventoryList, (*iter).PetSerial, *species);
						SetInventorySize(originInventoryList, (*iter).PetSerial, *species);
					}
					else {
						LoggerInstance().Warn("can not found species {0}", (*iter).PetHash);
					}
					++iter;
				}
			}
			void PetsInventory::SetInventorySize(PetInventoryList& list, const XRated::GlobalSerial petSerial, const Database::Info::Pet::Species& species)
			{
				if (std::find_if(list.begin(), list.end(), Inventory::FindCondition(petSerial)) == list.end()) {
					list.push_back(Inventory(petSerial, species.InventorySlotCount));
				}
				else {
					throw Exception("intited pet inventory size {0}", petSerial);
				}
			}
			bool PetsInventory::SetItemWithNoLock(PetInventoryList& list, const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, const Common::ItemEx& item)
			{
				PetInventoryList::iterator iter = std::find_if(list.begin(), list.end(), Inventory::FindCondition(petSerial));
				if (iter == list.end())
				{
					PetInfoRetrieveVisitor visitor(petSerial);
					userVisitor->PetVisitor(visitor);

					if (!visitor.GetPetInfo())
					{
						LoggerInstance().Error("SetItemWithNoLock Failed - visitor.GetPetInfo() == NULL({0},{1},{2})", petSerial, static_cast<int>(type), position);
						return false;
					}

					const XRated::Database::Info::Pet::Species* species = XRated::Database::DatabaseInstance().InfoCollections.Pets.GetSpecies(visitor.GetPetInfo()->PetHash);

					if (!species)
					{
						LoggerInstance().Error("SetItemWithNoLock Failed - Invalid PetHash {0}({1},{2},{3})", visitor.GetPetInfo()->PetHash, petSerial, static_cast<int>(type), position);
						list.push_back(Inventory(petSerial, XRated::Pet::MaxPetInventorySlotSize));
						iter = std::find_if(list.begin(), list.end(), Inventory::FindCondition(petSerial));
					}
					else
					{
						list.push_back(Inventory(petSerial, species->InventorySlotCount));
						iter = std::find_if(list.begin(), list.end(), Inventory::FindCondition(petSerial));
					}
				}

				if (iter->IsValidPosition(type, position) == false)
				{
					LoggerInstance().Error("SetItemWithNoLock Failed - iter->IsValidPosition({0},{1},{2})", petSerial, static_cast<int>(type), position);
					return false;
				}

				switch (type)
				{
				case XRated::PetItemSlot::PositionType::Equipment:
				{
					iter->Equip.at(position - Constants::Equipment::Pet_Mask) = item;
				}
				break;
				case XRated::PetItemSlot::PositionType::Inventory:
				{
					iter->Inven.at(position) = item;
				}
				break;
				default:
					LoggerInstance().Error(L"SetItem Failed, wrong pet item area type : {0}, itemHash : {1},itemCount : {2},itemInstance : ({3}:{4}), itemSerial : {5}", static_cast<int>(type), item.Info->Hash, static_cast<int>(item.StackedCount), item.InstanceEx, item.Serial);
					return false;
				}
				return true;
			}
			bool PetsInventory::SetItemWithNoLock(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, const Common::ItemEx& item)
			{
				return SetItemWithNoLock(inventoryList, petSerial, type, position, item);
			}
			bool PetsInventory::DropItemWithNoLock(PetInventoryList& list, const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, uint16 droppedCount)
			{
				PetInventoryList::iterator iter = std::find_if(list.begin(), list.end(), Inventory::FindCondition(petSerial));
				if (iter != list.end())
				{
					if (iter->IsValidPosition(type, position) == false)
						return false;

					switch (type)
					{
					case XRated::PetItemSlot::PositionType::Equipment:
					{
						iter->Equip.at(position - Constants::Equipment::Pet_Mask).Info = NULL;
					}
					break;
					case XRated::PetItemSlot::PositionType::Inventory:
					{
						if (droppedCount == 0)
							iter->Inven.at(position).Info = NULL;
						else
						{
							if (iter->Inven.at(position).StackedCount >= droppedCount) {
								iter->Inven.at(position).StackedCount -= droppedCount;
							}
							else {
								LoggerInstance().Error("item({0}) drop failed in petinventory({1},{2},{3}) - StackedCount({4}) is lower then droppedCount({5})",
									iter->Inven.at(position).Info->Hash, petSerial, static_cast<int>(type), position, iter->Inven.at(position).StackedCount, droppedCount);
									return false;
							}
							if (iter->Inven.at(position).StackedCount == 0)
								iter->Inven.at(position).Info = NULL;

						}

					}
					break;
					default:
						LoggerInstance().Error("PetInventory::DropItem Failed wrong pet item area typ, pet : {0}, area type : {1}", petSerial, static_cast<int>(type));
						return false;
					}
				}
				else {
					LoggerInstance().Error("PetInventory::DropItem Failed, can not found pet : {0}", petSerial);
					return false;
				}
				return true;
			}
			bool PetsInventory::DropItemWithNoLock(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, uint16 droppedCount)
			{
				return DropItemWithNoLock(inventoryList, petSerial, type, position, droppedCount);
			}

			const Common::ItemEx* PetsInventory::GetItem(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position) const
			{
				AutoLock lock(itemsCS);

				PetInventoryList::iterator iter = std::find_if(inventoryList.begin(), inventoryList.end(), Inventory::FindCondition(petSerial));
				if (iter == inventoryList.end()) {
					PetInfoRetrieveVisitor visitor(petSerial);
					userVisitor->PetVisitor(visitor);

					if (!visitor.GetPetInfo())
					{
						LoggerInstance().Error(L"GetItem Failed - visitor.GetPetInfo() == NULL({0},{1},{2})", petSerial, static_cast<int>(type), position);
						return NULL;
					}

					const XRated::Database::Info::Pet::Species* species = XRated::Database::DatabaseInstance().InfoCollections.Pets.GetSpecies(visitor.GetPetInfo()->PetHash);

					if (!species)
					{
						LoggerInstance().Error("GetItem Failed - Invalid PetHash {0}({1},{2},{3})", visitor.GetPetInfo()->PetHash, petSerial, static_cast<int>(type), position);
						return NULL;
					}

					inventoryList.push_back(Inventory(petSerial, species->InventorySlotCount));
					iter = std::find_if(inventoryList.begin(), inventoryList.end(), Inventory::FindCondition(petSerial));
				}

				if (iter->IsValidPosition(type, position) == false)
				{
					LoggerInstance().Error("GetItem Failed - iter->IsValidPosition({0},{1},{2})", petSerial, static_cast<int>(type), position);
					return NULL;
				}

				switch (type)
				{
				case XRated::PetItemSlot::PositionType::Equipment:
					return &(iter->Equip.at(position - Constants::Equipment::Pet_Mask));
				case XRated::PetItemSlot::PositionType::Inventory:
					return &(iter->Inven.at(position));
				default:
					LoggerInstance().Error("GetItem Failed - wrong area type({0},{1},{2})", petSerial, static_cast<int>(type), position);
					return NULL;
				}

				return nullptr;
			}

			bool PetsInventory::SetItem(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, const Common::ItemEx& item)
			{
				AutoLock lock(itemsCS);
				return SetItemWithNoLock(petSerial, type, position, item);
			}
			
			bool PetsInventory::DropItem(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, uint16 droppedCount)
			{
				AutoLock lock(itemsCS);
				return DropItemWithNoLock(petSerial, type, position, droppedCount);
			}

			void PetsInventory::AllChangedItemsAndOriginBackup(ChangedItems& changedItems, std::vector< Common::PetItemSlotEx >& allItems)
			{
				AutoLock lock(itemsCS);
				//�ϴ� ���� pet�� ���Ѱ��� üũ.
				PetInventoryList::iterator nowIter = inventoryList.begin();
				PetInventoryList::iterator nowEnd = inventoryList.end();
				while (nowIter != nowEnd) {
					PetInventoryList::iterator iter = std::find_if(originInventoryList.begin(), originInventoryList.end(), Inventory::FindCondition(nowIter->GetPetSerial()));
					if (iter != originInventoryList.end()) {
						//üũ �Ǿ��ٰ� ó��(origin���� deleteó�� �Ұ��� üũ�ϱ� ���ؼ�).
						//origin�� iter�� ó���ϴ� ������ �� �ڿ��� now�� ������ origin�� ���� ������̱� ������ origin�� ó���Ѵ�/now�� �׶��׶�ó��.
						iter->IsChecked = true;
						//���� ���� �����Ƿ� ���Ѵ�.
						ChangedSlotFilter(iter->Equip, nowIter->Equip, changedItems, nowIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Equipment);
						ChangedSlotFilter(iter->Inven, nowIter->Inven, changedItems, nowIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Inventory);
						//origin���� ����ó�� �Ѵ�.
						originInventoryList.erase(iter);
					}
					else {
						//origin�� �����Ƿ� ���� insert�̴�.
						GetAllItems(nowIter->Equip, nowIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Equipment, changedItems.Insert);
						GetAllItems(nowIter->Inven, nowIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Inventory, changedItems.Insert);
					}
					//snapshot�� ����� ������ ���� ���(now�� ��� item�� snapshot�� �����Ѵ�).
					GetAllItems(nowIter->Equip, nowIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Equipment, allItems);
					GetAllItems(nowIter->Inven, nowIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Inventory, allItems);
					++nowIter;
				}
				//origin���� ���� �ִ°��� delete.
				PetInventoryList::iterator originIter = originInventoryList.begin();
				PetInventoryList::iterator originEnd = originInventoryList.end();
				while (originIter != originEnd) {
					if (originIter->IsChecked == false) {
						//���� delete
						GetAllItems(originIter->Equip, originIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Equipment, changedItems.Delete);
						GetAllItems(originIter->Inven, originIter->GetPetSerial(), XRated::PetItemSlot::PositionType::Inventory, changedItems.Delete);
					}
					++originIter;
				}
				//���� ������ �ִ� ��� �������� allItems�� �־��ش�(snapshot��).
				//now�� origin�� backup
				originInventoryList = inventoryList;
			}

			void PetsInventory::ChangedSlotFilter(const Inventory::ItemSlots& origin, const Inventory::ItemSlots& now, ChangedItems& in, const GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type)
			{
				size_t max = now.size();
				Common::PetItemSlotEx itemSlot;
				itemSlot.PetSerial = petSerial;
				itemSlot.Type = type;
				uint8 position = 0;

				//pet equip�� ��� 0���� ������ �ƴ϶� petEquip_1���� �����Ѵ�.
				if (type == XRated::PetItemSlot::PositionType::Equipment) {
					position = XRated::Constants::Equipment::Pet_Mask;
				}

				for (size_t i = 0; i < max; ++i, ++position)
				{
					const Common::ItemEx& originSlot(origin.at(i));
					const Common::ItemEx& nowSlot(now.at(i));
					if ((originSlot != nowSlot) && ((originSlot.Info != NULL) || (nowSlot.Info != NULL))) {

						if (nowSlot.Info == NULL) {
							//drop���� üũ.					
							itemSlot.ItemInfo = originSlot.Info;
							itemSlot.ItemHash = originSlot.Info->Hash;
							itemSlot.Serial = originSlot.Serial;
							itemSlot.Position = position;
							itemSlot.Stacked = originSlot.StackedCount;
							itemSlot.InstanceEx = originSlot.InstanceEx;


							in.Delete.push_back(itemSlot);
						}
						else if (originSlot.Info == NULL) {
							//insert���� üũ.					
							itemSlot.ItemInfo = nowSlot.Info;
							itemSlot.ItemHash = nowSlot.Info->Hash;
							itemSlot.Serial = nowSlot.Serial;
							itemSlot.Position = position;
							itemSlot.Stacked = nowSlot.StackedCount;
							itemSlot.InstanceEx = nowSlot.InstanceEx;

							in.Insert.push_back(itemSlot);
						}
						else {
							//update���� üũ.					
							itemSlot.ItemInfo = nowSlot.Info;
							itemSlot.ItemHash = nowSlot.Info->Hash;
							itemSlot.Serial = nowSlot.Serial;
							itemSlot.Position = position;
							itemSlot.Stacked = nowSlot.StackedCount;
							itemSlot.InstanceEx = nowSlot.InstanceEx;

							in.Update.push_back(itemSlot);
						}
					}
				}
			}

			void PetsInventory::GetAllItems(const Inventory::ItemSlots& source, const GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, std::vector< Common::PetItemSlotEx >& all)
			{
				Common::PetItemSlotEx itemSlot;
				itemSlot.Type = type;
				itemSlot.PetSerial = petSerial;
				Inventory::ItemSlots::const_iterator iter = source.begin();
				Inventory::ItemSlots::const_iterator end = source.end();
				uint32 position = 0;
				//pet equip�� ��� 0���� ������ �ƴ϶� petEquip_1���� �����Ѵ�.
				if (type == XRated::PetItemSlot::PositionType::Equipment) {
					position = XRated::Constants::Equipment::Pet_Mask;
				}
				while (iter != end) {
					const Common::ItemEx& slot(*iter);
					if (slot.Info != NULL) {
						itemSlot.ItemInfo = slot.Info;
						itemSlot.ItemHash = slot.Info->Hash;
						itemSlot.Serial = slot.Serial;
						itemSlot.Position = static_cast<uint8>(position);
						itemSlot.Stacked = slot.StackedCount;
						itemSlot.InstanceEx = slot.InstanceEx;

						all.push_back(itemSlot);
					}

					++position;
					++iter;
				}
			}

			void PetsInventory::GetAllItems(const GlobalSerial& petSerial, std::vector<Common::PetItemSlotEx>& petItems)
			{
				PetInventoryList::const_iterator itr = std::find_if(inventoryList.begin(), inventoryList.end(), Inventory::FindCondition(petSerial));

				if (itr == inventoryList.end())
					return;

				Common::PetItemSlotEx itemSlot;

				itemSlot.PetSerial = petSerial;

				/// Pet Equipment
				itemSlot.Type = XRated::PetItemSlot::PositionType::Equipment;

				/// �������� �� �� ��� �ε����� std::vector<Common::ItemEx>::const_iterator equipItr = itr->Equip.begin(); equipItr != itr->Equip.end(); ++eqiupItr)
				for (uint16 i = 0; i < itr->Equip.size(); ++i)
				{
					if (itr->Equip[i].Info == NULL)
						continue;

					itemSlot.InstanceEx = itr->Equip[i].InstanceEx;
					itemSlot.ItemHash = itr->Equip[i].Info->Hash;
					itemSlot.ItemInfo = itr->Equip[i].Info;
					itemSlot.Position = i + static_cast<int>(Constants::Equipment::Pet_Mask);
					itemSlot.Serial = itr->Equip[i].Serial;
					itemSlot.Stacked = itr->Equip[i].StackedCount;

					petItems.push_back(itemSlot);
				}

				///// Pet Inventory
				itemSlot.Type = XRated::PetItemSlot::PositionType::Inventory;

				for (uint16 i = 0; i < itr->inventorySize; ++i)
				{
					if (itr->Inven[i].Info == NULL)
						continue;

					itemSlot.InstanceEx = itr->Inven[i].InstanceEx;
					itemSlot.ItemHash = itr->Inven[i].Info->Hash;
					itemSlot.ItemInfo = itr->Inven[i].Info;
					itemSlot.Position = i;
					itemSlot.Serial = itr->Inven[i].Serial;
					itemSlot.Stacked = itr->Inven[i].StackedCount;

					petItems.push_back(itemSlot);
				}
			}

			Common::PetItemSlotEx PetsInventory::FindItem(uint32 itemHash, bool onlyAppear) const
			{
				Common::PetItemSlotEx resultSlot;

				resultSlot.Type = XRated::PetItemSlot::PositionType::Invaild;

				for (std::vector<Inventory>::const_iterator itr = inventoryList.begin(); itr != inventoryList.end(); ++itr)
				{
					if (onlyAppear == true)
					{
						PetInfoRetrieveVisitor visitor(itr->PetSerial);
						userVisitor->PetVisitor(visitor);

						if (visitor.GetPetInfo() == NULL || visitor.GetPetInfo()->Appear == false)
							continue;
					}

					for (uint16 i = 0; i < itr->Equip.size(); ++i)
					{
						if (itr->Equip[i].Info == NULL)
							continue;

						if (itr->Equip[i].Info->Hash == itemHash)
						{
							resultSlot.PetSerial = itr->PetSerial;
							resultSlot.Type = XRated::PetItemSlot::PositionType::Equipment;
							resultSlot.InstanceEx = itr->Equip[i].InstanceEx;
							resultSlot.ItemHash = itr->Equip[i].Info->Hash;
							resultSlot.ItemInfo = itr->Equip[i].Info;
							resultSlot.Position = i + static_cast<int>(Constants::Equipment::Pet_Mask);
							resultSlot.Serial = itr->Equip[i].Serial;
							resultSlot.Stacked = itr->Equip[i].StackedCount;

							return resultSlot;
						}
					}

					for (uint16 i = 0; i < itr->inventorySize; ++i)
					{
						if (itr->Inven[i].Info == NULL)
							continue;

						if (itr->Inven[i].Info->Hash == itemHash)
						{
							resultSlot.PetSerial = itr->PetSerial;
							resultSlot.Type = XRated::PetItemSlot::PositionType::Inventory;
							resultSlot.InstanceEx = itr->Inven[i].InstanceEx;
							resultSlot.ItemHash = itr->Inven[i].Info->Hash;
							resultSlot.ItemInfo = itr->Inven[i].Info;
							resultSlot.Position = i;
							resultSlot.Serial = itr->Inven[i].Serial;
							resultSlot.Stacked = itr->Inven[i].StackedCount;

							return resultSlot;
						}
					}
				}

				return resultSlot;
			}

			Common::PetItemSlotEx PetsInventory::FindItemPosition(const Common::ItemEx::IFindCondition& condition) const
			{
				Common::PetItemSlotEx resultSlot;

				for (std::vector<Inventory>::const_iterator itr = inventoryList.begin(); itr != inventoryList.end(); ++itr)
				{
					for (uint16 i = 0; i < itr->Equip.size(); ++i)
					{
						if (itr->Equip[i].Info == NULL)
							continue;

						if (condition(itr->Equip[i]) == true)
						{
							resultSlot.PetSerial = itr->PetSerial;
							resultSlot.Type = XRated::PetItemSlot::PositionType::Equipment;
							resultSlot.InstanceEx = itr->Equip[i].InstanceEx;
							resultSlot.ItemHash = itr->Equip[i].Info->Hash;
							resultSlot.ItemInfo = itr->Equip[i].Info;
							resultSlot.Position = i + static_cast<int>(Constants::Equipment::Pet_Mask);
							resultSlot.Serial = itr->Equip[i].Serial;
							resultSlot.Stacked = itr->Equip[i].StackedCount;

							return resultSlot;
						}
					}

					for (uint16 i = 0; i < itr->inventorySize; ++i)
					{
						if (itr->Inven[i].Info == NULL)
							continue;

						if (condition(itr->Inven[i]) == true)
						{
							resultSlot.PetSerial = itr->PetSerial;
							resultSlot.Type = XRated::PetItemSlot::PositionType::Inventory;
							resultSlot.InstanceEx = itr->Inven[i].InstanceEx;
							resultSlot.ItemHash = itr->Inven[i].Info->Hash;
							resultSlot.ItemInfo = itr->Inven[i].Info;
							resultSlot.Position = i;
							resultSlot.Serial = itr->Inven[i].Serial;
							resultSlot.Stacked = itr->Inven[i].StackedCount;

							return resultSlot;
						}
					}
				}

				resultSlot.Type = XRated::PetItemSlot::PositionType::Invaild;

				return resultSlot;
			}

			bool PetsInventory::IsItemInPetInventory(const GlobalSerial petSerial) const
			{
				PetInventoryList::const_iterator iter = std::find_if(inventoryList.begin(), inventoryList.end(), Inventory::FindCondition(petSerial));
				if (iter != inventoryList.end()) {
					return (*iter).IsHaveItem();
				}
				return false;
			}

			uint32 PetsInventory::DeleteItems(uint32 itemHash, uint32 count, bool onlyAppear)
			{
				uint32 droppedCount = 0;

				for (std::vector<Inventory>::iterator itr = inventoryList.begin(); itr != inventoryList.end() && droppedCount < count; ++itr)
				{
					if (onlyAppear == true)
					{
						PetInfoRetrieveVisitor visitor(itr->PetSerial);
						userVisitor->PetVisitor(visitor);

						if (visitor.GetPetInfo() == NULL || visitor.GetPetInfo()->Appear == false)
							continue;
					}

					for (uint16 i = 0; i < itr->Equip.size() && droppedCount < count; ++i)
					{
						if (itr->Equip[i].Info == NULL)
							continue;

						if (itr->Equip[i].Info->Hash == itemHash)
						{
							itr->Equip[i].Info = 0;
							itr->Equip[i].StackedCount = 0;

							++droppedCount;
						}
					}

					for (uint16 i = 0; i < itr->inventorySize && droppedCount < count; ++i)
					{
						if (itr->Inven[i].Info == NULL)
							continue;

						if (itr->Inven[i].Info->Hash == itemHash)
						{
							if (itr->Inven[i].StackedCount >= count - droppedCount)
							{
								itr->Inven[i].StackedCount -= (count - droppedCount);
								droppedCount += (count - droppedCount);
							}
							else
							{
								droppedCount += itr->Inven[i].StackedCount;

								itr->Inven[i].StackedCount = 0;
							}

							if (itr->Inven[i].StackedCount == 0)
								itr->Inven[i].Info = 0;
						}
					}
				}

				return droppedCount;
			}

			int PetsInventory::GetItemCount(uint32 itemHash, bool onlyAppear) const
			{
				int count = 0;

				for (std::vector<Inventory>::const_iterator itr = inventoryList.begin(); itr != inventoryList.end(); ++itr)
				{
					if (onlyAppear == true)
					{
						PetInfoRetrieveVisitor visitor(itr->PetSerial);
						userVisitor->PetVisitor(visitor);

						if (visitor.GetPetInfo() == NULL || visitor.GetPetInfo()->Appear == false)
							continue;
					}

					for (uint16 i = 0; i < itr->Equip.size(); ++i)
					{
						if (itr->Equip[i].Info == NULL)
							continue;

						if (itr->Equip[i].Info->Hash == itemHash)
							count += itr->Equip[i].StackedCount;
					}

					for (uint16 i = 0; i < itr->inventorySize; ++i)
					{
						if (itr->Inven[i].Info == NULL)
							continue;

						if (itr->Inven[i].Info->Hash == itemHash)
							count += itr->Inven[i].StackedCount;
					}
				}

				return count;
			}

			int PetsInventory::GetItemCount(const Common::ItemEx::IFindCondition& condition, bool onlyAppear) const
			{
				int count = 0;

				for (std::vector<Inventory>::const_iterator itr = inventoryList.begin(); itr != inventoryList.end(); ++itr)
				{
					if (onlyAppear == true)
					{
						PetInfoRetrieveVisitor visitor(itr->PetSerial);
						userVisitor->PetVisitor(visitor);

						if (visitor.GetPetInfo() == NULL || visitor.GetPetInfo()->Appear == false)
							continue;
					}

					for (uint16 i = 0; i < itr->Equip.size(); ++i)
					{
						if (itr->Equip[i].Info == NULL)
							continue;

						if (condition(itr->Equip[i]) == true)
							++count;
					}

					for (uint16 i = 0; i < itr->inventorySize; ++i)
					{
						if (itr->Inven[i].Info == NULL)
							continue;

						if (condition(itr->Inven[i]) == true)
							count += itr->Inven[i].StackedCount;
					}
				}

				return count;
			}

			int PetsInventory::GetItemCount(const Database::Info::ItemInfo::IFindCondition& condition, bool onlyAppear) const
			{
				int count = 0;

				for (std::vector<Inventory>::const_iterator itr = inventoryList.begin(); itr != inventoryList.end(); ++itr)
				{
					if (onlyAppear == true)
					{
						PetInfoRetrieveVisitor visitor(itr->PetSerial);
						userVisitor->PetVisitor(visitor);

						if (visitor.GetPetInfo() == NULL || visitor.GetPetInfo()->Appear == false)
							continue;
					}

					for (uint16 i = 0; i < itr->Equip.size(); ++i)
					{
						if (itr->Equip[i].Info == NULL)
							continue;

						if (condition(*itr->Equip[i].Info) == true)
							++count;
					}

					for (uint16 i = 0; i < itr->inventorySize; ++i)
					{
						if (itr->Inven[i].Info == NULL)
							continue;

						if (condition(*itr->Inven[i].Info) == true)
							count += itr->Inven[i].StackedCount;
					}
				}

				return count;
			}


		}
	}
}