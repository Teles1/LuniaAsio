#include "Info.h"
#include <Core/Utils/ConfigReader.h>

namespace Lunia {
	namespace XRated {
		namespace Database {

			void LuniaDatabase::Init() {
				InfoCollections.Npcs.Init(false);
				InfoCollections.Pets.Init(false);
				InfoCollections.Stages.Init(false);
				InfoCollections.StageGroups.Init(false);
				InfoCollections.StageGroups.SetStageInfo(&InfoCollections.Stages);
				//InfoCollections.Stages.Init(false);
				//InfoCollections.npcs.Init(false);
				//InfoCollections.stateBundles.Init(false);
				//InfoCollections.itemInfos.Init(false);
			}

			LuniaDatabase& LuniaDatabase::GetInstance()
			{
				static LuniaDatabase m_instance;
				return m_instance;
			}

			LuniaDatabase& DatabaseInstance() {
				return LuniaDatabase::GetInstance();
			}
		}
	}
}