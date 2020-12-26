#pragma once

#include <Core/Core.h>
#include <Info/Info/Database.h>
#include <Info/Info/Items/CompressedItemInfoManager.h>
#include <Info/Info/GlobalInfo.h>
#include <Info/Info/NPCs/NPCManager.h>
#include <Info/Info/Actions/ActionsManager.h>
#include <Info/Info/StateBundles/StateBundlesManager.h>
#include <Info/Info/StageGroup/StageGroupManager.h>
#include <Info/Info/StageInfo/StageInfoManager.h>
#include <Info/Info/Enchant/EnchantTable.h>
#include <Info/Info/EquipSet/EquipSetManager.h>
#include <Info/Info/Pets/PetsManager.h>

namespace Lunia {
	namespace XRated {
		namespace Database {

			class LuniaDatabase {
				public:
					void Init();

					struct Collection {
						Info::StageGroupManager StageGroups;
						Info::NPCInfoManager Npcs;
						Enchant::EnchantTableManager EnchantTables;
						//Info::StageInfoManager stageInfos;
						//Info::ActionInfoManager actions;
						Info::StateBundleInfoManager StateBundles;
						Info::CompressedItemInfoManager Items;
						Info::EquipSetManager EquipSet;
						Info::PetManager Pets;
					} InfoCollections;
			};
			LuniaDatabase& DatabaseInstance();
		}
	}
}