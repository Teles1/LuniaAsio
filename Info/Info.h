#pragma once

#include <Core/Core.h>
#include "./Info/Database.h"
#include <Info/Info/Items/CompressedItemInfoManager.h>
#include "Info/GlobalInfo.h"
#include "Info/NPCs/NPCManager.h"
#include "Info/Actions/ActionsManager.h"
#include "Info/StateBundles/StateBundlesManager.h"
#include "Info/StageGroup/StageGroupManager.h"
#include "Info/StageInfo/StageInfoManager.h"
#include <Info/Info/Enchant/EnchantTable.h>
#include <Info/Info/EquipSet/EquipSetManager.h>
#include <Info/Info/Pets/PetsManager.h>
#include <Info/Info/PvpRewardItemInfos/PvPRewardItemInfosManager.h>
#include <Info/Info/Event/EventInfoManager.h>
#include <Info/Info/StylePoints/StylePointInfoManager.h>

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
						Info::StateBundleInfoManager StateBundles;
						Info::CompressedItemInfoManager Items;
						Info::EquipSetManager EquipSet;
						Info::PetManager Pets;
						Info::StageInfoManager StageInfos;
						Info::ActionInfoManager Actions;
						Info::PvPRewardItemTable PvPRewardItemInfos;
						Info::EventInfoManager EventInfoManager;
						Info::StylePointInfoManager StylePoints;
					} InfoCollections;
			};
			LuniaDatabase& DatabaseInstance();
		}
	}
}