#pragma once
#include <StageServer/Common.h>
#include <vector>

namespace Lunia
{
	namespace XRated
	{
		namespace StageServer {

			class Equip
			{
			public:
				Equip();
				virtual ~Equip();

				bool SetItems(uint8 set, uint32 pos, const Common::ItemEx& item);
				bool PushItems(uint32 pos, const Common::ItemEx& item);
				void DeleteItem(uint32 pos);
				void MoveItems(uint32 fromPos, uint32 toPos);

				void Clear();

				bool ChangeEquipSet(uint8 set);
				bool ChangeCashEquipSet(uint8 set);
				uint8 GetCurrentEquipSet();
				uint8 GetCurrentCashEquipSet();
				bool IsCurrentEquipSet(uint8 set, uint32 pos);
				void GetEquipBags(std::vector<uint8>& bags);

				const Common::ItemEx* GetItem(uint32 pos) const; ///< @return NULL if pos is invalid or empty slot
				bool SetItemInstance(uint32 pos, InstanceEx instance);


				void GetEquip(std::vector<Common::ItemEx>& items) const;// const { return equip; }
				void GetNormalEquipments(std::vector< XRated::EquippedItem >& list);
				void GetCashEquipments(std::vector< XRated::EquippedItem >& list);
				void AllItems(std::vector< Common::ItemSlotEx >& allItems);

				XRated::ItemPosition FindItem(XRated::GlobalSerial itemSerial) const;

			private:
				bool IsCashEquipment(XRated::Constants::Equipment position) const;
				bool IsEquipment(XRated::Constants::Equipment position) const;
				bool IsEtcEquipments(XRated::Constants::Equipment position) const;

				uint8 currentEquipSet;
				uint8 currentCashEquipSet;

				typedef std::map< XRated::Constants::Equipment, Common::ItemEx > Equipments;
				typedef std::map< uint8, Equipments > EquipSet;

				EquipSet equipSet;
				EquipSet cashEquipSet;
				Equipments etcEquips;
			};


		}
	}
}
