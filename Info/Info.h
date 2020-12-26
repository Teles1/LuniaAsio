#pragma once

#include <Core/Core.h>
#include "./Info/Database.h"
#include "Info/GlobalInfo.h"
#include "Info/NPCs/NPCManager.h"
#include "Info/Actions/ActionsManager.h"
#include "Info/StateBundles/StateBundlesManager.h"
#include "Info/Items/CompressedItemInfoManager.h"
#include "Info/StageGroup/StageGroupManager.h"
#include "Info/StageInfo/StageInfoManager.h"
#include <Info/Info/Enchant/EnchantTable.h>

namespace Lunia {
	namespace XRated {
		namespace Database {

			class LuniaDatabase {
				public:
					void Init();

					struct Collection {
						Info::StageGroupManager stageGroups;
						Info::NPCInfoManager npcs;
						Info::Enchant::EnchantTableManager enchantManager;
						//Info::StageInfoManager stageInfos;
						//Info::ActionInfoManager actions;
						//Info::StateBundleInfoManager stateBundles;
						//Info::CompressedItemInfoManager itemInfos;
					} InfoCollections;
			};
			LuniaDatabase& DatabaseInstance();
		}
	}
}