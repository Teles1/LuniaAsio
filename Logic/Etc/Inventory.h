#pragma once

#include "Equipments.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class Inventory {
	//public :
	//	const static float COOLDOWN;
	private :
		Equipments equipments;
		struct ItemCooldown {
			uint32 id;
			float time;
			ItemCooldown(uint32 i, float t) : id(i), time(t) {}
		};
		std::list<ItemCooldown> cooldowns;
		std::vector<float> categoryCooldown;

		float cooldown;

	public :
		Inventory();
		~Inventory() {}

		bool IsAbleToUse(const Database::Info::ItemInfo* info, Constants::GameTypes gameType, Player* player, const Database::Info::StageInfo* stageInfo = NULL);

		void Init();

		uint32 UseItem(const Database::Info::ItemInfo* info, Constants::GameTypes gameType, Player* player, const Database::Info::StageInfo* stageInfo);

		bool Equip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, Player* obj, bool bForce, uint16 RebirthCountEquipLimit=0);
		bool Unequip( Constants::Equipment where, uint32& equipmentOld, Player* obj );
		bool ChangeEquipToEquip(Constants::Equipment from, Constants::Equipment to);
		bool IsEnableEquipment(Constants::Equipment where);
		void ValidateEquippedItems(Player* obj, uint16 RebirthCountEquipLimit=0);

		//bool UnEquip(Constants::Equipment fromWhere, Player* obj);
		inline uint32 GetEquipmentId(Constants::Equipment part) {
			return equipments.GetEquipmentId(part);
		}
		bool PetEquip(const XRated::Pet& pet,  const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, Player* obj, bool bForce);
		void AddEquipSetBonus(Player* obj);
		void Update(float dt);
	};


}	}	}
