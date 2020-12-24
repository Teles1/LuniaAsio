#pragma once

#include <Core/Core.h>
#include "./Info/Database.h"
#include "Info/GlobalInfo.h"
#include "Info/NPCs/NPCManager.h"
#include "Info/Actions/ActionsManager.h"
#include "Info/StateBundles/StateBundlesManager.h"
#include "Info/Items/CompressedItemInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {

			class LuniaDatabase {
				public:
					void Init();

					struct Collection {
						//Info::CompressedActionInfoManager compressed;
						//Info::CompressedItemInfoManager itemInfos;
						//Info::ActionInfoManager actions;
						//Info::NPCInfoManager npcs;
						//Info::StateBundleInfoManager stateBundles;
						Info::CompressedItemInfoManager itemInfos;
					} InfoCollections;
			};

			LuniaDatabase& DatabaseInstance();
		}
	}
}