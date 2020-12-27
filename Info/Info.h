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
#include <Info/Info/PvPRankRewardItemInfos/PvPRankRewardItemTable.h>
#include <Info/Info/Event/EventInfoManager.h>
#include <Info/Info/StylePoints/StylePointInfoManager.h>
#include <Info/Info/GeneralInfos/GeneralInfoManager.h>
#include <Info/Info/NonPlayerModifier/NonPlayerModifierManager.h>
#include <Info/Info/StatusInfo/IndexedStatusInfoManager.h>
#include <Info/Info/Projectiles/IndexedProjectileInfoManager.h>
#include <Info/Info/MISCS/IndexedMISCSManager.h>
#include <Info/Info/Skills/SkillInfoManager.h>
#include <Info/Info/EventNonPlayerItemList/EventNonPlayerItemListManager.h>
#include <Info/Info/StagePlayTime/StagePlayTimeManager.h>
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
					Info::ActionInfoManager Actions;
					Info::NPCInfoManager Npcs;
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
					Info::IndexedProjectileInfoManager Projectiles;
					Info::IndexedMISCInfoManager Miscs;
				} InfoCollections;
			};
			LuniaDatabase& DatabaseInstance();
		}
	}
}