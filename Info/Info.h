#ifndef Info_H
#define Info_H
#pragma once

#include <Core/Core.h>
#include "./Info/Database.h"
#include <Info/Info/Actions/CompressedActionsManager.h>
#include <Info/Info/Items/CompressedItemInfoManager.h>
#include <Info/Info/Items/BagInfos.h>
#include <Info/Info/Items/LuniaChipInfoManager.h>
#include <Info/Info/Items/DailyItemInfoManager.h>
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
#include <Info/Info/GuildFamily/GuildInfoManager.h>
#include <Info/Info/GuildFamily/FamilyInfoManager.h>
#include <Info/Info/CashShopBarter/CashShopBarterManager.h>
#include <Info/Info/StageInfo/StageLicenseProvider.h>
#include <Info/Info/ETC/EpisodeSquareAdventageManager.h>
#include <Info/Info/ETC/FishingInfoManager.h>
namespace Lunia {
	namespace XRated {
		namespace Database {

			class LuniaDatabase {
			public:
				void Init();
				static LuniaDatabase& GetInstance();
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
					Info::GuildInfoManager GuildInfos;
					Info::CashStampInfoManager CashStamps;
					Info::BarterItemInfoManager BarterItems;
					Info::FamilyInfoManager FamilyInfos;
					Info::StageLicenseProvider LicenseProvider;
					Info::EpisodeSquareAdventageManager EpisodeSquareAdventageInfos;
					Info::FishingInfoManager FishigInfos;
					Info::BagInfos BagInfos;
					Info::LuniaChipInfoManager LuniaChipItems;
					Info::DailyItemInfoManager DailyItems;
				} InfoCollections;
			};
			LuniaDatabase& DatabaseInstance();
		}
	}
}
#endif // !Info_H