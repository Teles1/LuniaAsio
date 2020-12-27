#pragma once

#include "../State/State.h"
#include <Info/Info/Enchant/Enchant.h>

namespace Lunia { namespace XRated {	namespace Logic {

	class Player;

	class Equipments {
	public :
		struct Equipment {
			uint32 name;
			bool enable;
			std::vector<Stat::IState*> states;
			std::vector<int> levelConditions;
			int64 instance;
		};

	protected :
		Equipment parts[XRated::Constants::Equipment::Cnt];

	public :
		Equipments();
		~Equipments();

		void Init();

		bool IsEquipped(uint32 name) const;
		bool IsAbleToEquip(const Database::Info::ItemInfo* info, Player* obj, uint16 RebirthCountEquipLimit=0) const;
		uint32 GetEquipmentId(Lunia::XRated::Constants::Equipment part, bool isCheckEnable = false) const;

		void Equip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, Player* obj, uint16 RebirthCountEquipLimit=0);
        bool UnEquip(Constants::Equipment fromWhere, uint32& equipmentOld, Player* obj);
		bool ChangeEquipToEquip(Constants::Equipment from, Constants::Equipment to);
		bool IsEnableEquipment(Constants::Equipment where);
		void ValidateEquippedItems(Player* obj, uint16 RebirthCountEquipLimit);

		void AddEnchantState(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, Player* obj, uint16 RebirthCountEquipLimit=0);
		void AddEquipSetBonus(Player* obj);
		void RemoveEquipSetBonus(Player* obj, uint32 equipmentHash);
		void GetEquipList( std::vector<uint32>& list );
	private:
		struct PetParts {
			XRated::GlobalSerial	PetSerial;
			Equipment Parts[XRated::Constants::Equipment::MaxPetEquipCnt];

			struct Finder
			{
				XRated::GlobalSerial serial;
				Finder(XRated::GlobalSerial petSerial) : serial(petSerial) {}
				inline bool operator()(const PetParts& p) const { return p.PetSerial==serial; }
			};
			PetParts(XRated::GlobalSerial petSerial);
		};
		typedef std::vector< PetParts > PetsParts;

		PetsParts	petsParts;
	public:
		uint32 GetPetEquipmentId(XRated::GlobalSerial petSerial, Lunia::XRated::Constants::Equipment where) const;
		bool IsPetAbleToEquip(const Database::Info::ItemInfo* info, uint32 petHash) const;
		void PetEquip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, Player* obj, XRated::GlobalSerial petSerial);
        bool PetUnEquip(Constants::Equipment fromWhere, uint32& equipmentOld, Player* obj, XRated::GlobalSerial petSerial);
		void PetUnEquipAll(XRated::GlobalSerial petSerial, Player* obj);
	private:
		bool PetUnEquip(Constants::Equipment fromWhere, uint32& equipmentOld, Player* obj, PetParts& petParts);

		void Equip(const Database::Info::ItemInfo* info, int64 fields, Equipments::Equipment& equipment, Player* obj, uint16 rebirthCountEquipLimit=0);
		bool UnEquip(uint32& equipmentOld, Player* obj, Equipments::Equipment& equipment);

	};

}	}	}
