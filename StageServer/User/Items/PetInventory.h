#pragma once
#include <Info/Info.h>
#include <StageServer/Protocol/FromServer.h>
#include <StageServer/Common.h>
namespace Lunia
{
	namespace XRated
	{
		namespace StageServer {

			class User;
			class IUserRoom;

			class IPetItemVisitor
			{
			public:
				IPetItemVisitor() { }
				virtual ~IPetItemVisitor() { }

				virtual bool Run(User*, IUserRoom*) = 0;
			};

			class IPetItemVisitorListener
			{
			public:
				virtual bool PetVisitor(IPetItemVisitor& visitor) = 0;
			};

			class PetsInventory
			{
			public:
				struct ChangedItems {
					std::vector< Common::PetItemSlotEx >	Insert;
					std::vector< Common::PetItemSlotEx >	Update;
					std::vector< Common::PetItemSlotEx >	Delete;

					void Clear();
				};
			private:
				struct Inventory
				{
					typedef std::vector<Common::ItemEx> ItemSlots;

					XRated::GlobalSerial PetSerial;

					ItemSlots Equip;
					ItemSlots Inven;
					int inventorySize;
					bool IsChecked;

					const XRated::GlobalSerial& GetPetSerial() const;

					class FindCondition
					{
						XRated::GlobalSerial serial;
					public:
						FindCondition(const XRated::GlobalSerial& in) : serial(in) {}
						bool operator()(const Inventory& rhs) const { return (serial == rhs.GetPetSerial()) ? true : false; }
					};

					bool IsValidPosition(XRated::PetItemSlot::PositionType type, uint8 position);
					bool IsHaveItem() const;

					Inventory(const XRated::GlobalSerial& pet, int InventorySize);
					~Inventory();
				};

				typedef std::vector< Inventory > PetInventoryList;
				mutable PetInventoryList	inventoryList;
				PetInventoryList	originInventoryList;

				mutable std::mutex itemsCS;

				IPetItemVisitorListener* userVisitor;
			public:
				void Init();
				int SetItemList(const std::string& items, std::vector< XRated::Pet >& pets, Protocol::FromServer::ListPetItem& packet/*output*/);
				void AddInventory(const XRated::Pet& pet);

				bool SetItem(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, const Common::ItemEx& item);
				bool DropItem(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, uint16 droppedCount = 0);
				const Common::ItemEx* GetItem(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position) const;
				void AllChangedItemsAndOriginBackup(ChangedItems& changedItems, std::vector< Common::PetItemSlotEx >& allItems);
				void GetAllItems(const XRated::GlobalSerial& petSerial, std::vector<Common::PetItemSlotEx>& petItems);

				Common::PetItemSlotEx FindItem(uint32 itemHash, bool onlyAppear) const;
				Common::PetItemSlotEx FindItemPosition(const Common::ItemEx::IFindCondition& condition) const;

				std::mutex& GetSync() { return itemsCS; }

				bool IsItemInPetInventory(const GlobalSerial petSerial) const;

				uint32 DeleteItems(uint32 itemHash, uint32 count, bool onlyAppear); ///< @return deleted item count

				int GetItemCount(uint32 itemHash, bool onlyAppear) const;
				int GetItemCount(const Common::ItemEx::IFindCondition& condition, bool onlyAppear = true) const;
				int GetItemCount(const Database::Info::ItemInfo::IFindCondition& condition, bool onlyAppear = true) const;

			private:
				bool SetItemWithNoLock(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, const Common::ItemEx& item);
				bool DropItemWithNoLock(const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, uint16 droppedCount = 0);
			private:
				bool SetItemWithNoLock(PetInventoryList& list, const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, const Common::ItemEx& item);
				bool DropItemWithNoLock(PetInventoryList& list, const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, uint8 position, uint16 droppedCount = 0);

				void ChangedSlotFilter(const Inventory::ItemSlots& origin, const Inventory::ItemSlots& now, ChangedItems& in, const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type);
				void GetAllItems(const Inventory::ItemSlots& source, const XRated::GlobalSerial& petSerial, XRated::PetItemSlot::PositionType type, std::vector< Common::PetItemSlotEx >& all);

				void SetPetsInventorySize(const std::vector<XRated::Pet>& pets);

				void SetInventorySize(PetInventoryList& list, const XRated::GlobalSerial petSerial, const Database::Info::Pet::Species& species);
			public:
				PetsInventory(IPetItemVisitorListener* userVisitor);
				~PetsInventory();
			};

		}
	}
}