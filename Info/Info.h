#pragma once

#include <Core/Core.h>
#include "./Info/Database.h"
#include <Info/Info/Actions/CompressedActionsManager.h>
#include <Info/Info/Items/CompressedItemInfoManager.h>
#include "Info/GlobalInfo.h"
#include "Info/NPCs/CompressedNPCManager.h"
#include "Info/StateBundles/StateBundlesManager.h"
#include "Info/StageGroup/StageGroupManager.h"
#include "Info/StageInfo/StageInfoManager.h"
#include <Info/Info/Enchant/EnchantTable.h>
#include <Info/Info/EquipSet/EquipSetManager.h>
#include <Info/Info/Pets/PetsManager.h>
#include <Info/Info/PvpRewardItemInfos/PvPRewardItemInfosManager.h>
#include <Info/Info/PvPRankRewardItemInfos/PvPRankRewardItemTable.h>
#include <Info/Info/Event/EventInfoManager.h>
#include <Info/Info/StylePoints/StylePointInfoManager.h>
#include <Info/Info/GeneralInfos/GeneralInfoManager.h>
#include <Info/Info/NonPlayerModifier/NonPlayerModifierManager.h>
#include <Info/Info/StatusInfo/IndexedStatusInfoManager.h>
#include <Info/Info/Projectiles/CompressedProjectileInfoManager.h>
#include <Info/Info/MISCS/IndexedMISCSManager.h>
#include <Info/Info/Skills/SkillInfoManager.h>
#include <Info/Info/EventNonPlayerItemList/EventNonPlayerItemListManager.h>
#include <Info/Info/StagePlayTime/StagePlayTimeManager.h>
#include <Info/Info/UpgradeTables/CompressedUpgradeTable.h>
#include <Info/Info/HashTable.h>

namespace Lunia {
	namespace XRated {
		namespace Database {

			class LuniaDatabase {
			private:
			public:
				void Init();

				struct Collection {
					Info::SkillInfoManager Skills;
					Info::StateBundleInfoManager StateBundles;
					Enchant::EnchantTableManager EnchantTables;
					Info::CompressedItemInfoManager Items;
					Info::EquipSetManager EquipSet;
					Info::PetManager Pets;
					Info::CompressedActionInfoManager Actions;
					Info::CompressedNPCInfoManager Npcs;
					Info::StageInfoManager Stages;
					Info::EventNonPlayerItemList EventNonPlayerItemLists;
					Info::StageGroupManager StageGroups;
					Info::IndexedStatusInfoManager Statuses;
					Info::StagePlayTimeInfoManager StagePlayTimes;
					Info::PvPRewardItemTable PvPRewardItemInfos;
					Info::PvPRankRewardItemTable PvPRankRewardItemInfos;
					Info::EventInfoManager EventInfoManager;
					Info::StylePointInfoManager StylePoints;
					Info::GeneralInfoManager GeneralInfos;
					Info::NonPlayerModifierTable NonPlayerModifierInfos;
					Info::CompressedProjectileInfoManager Projectiles;
					Info::IndexedMISCInfoManager Miscs;
					Info::CompressedUpgradeTableManager UpgradeTables;
				} InfoCollections;
			};
			LuniaDatabase& DatabaseInstance();
		}
	}
}