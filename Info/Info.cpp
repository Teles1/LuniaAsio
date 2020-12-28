#include "Info.h"
#include <Core/Utils/ConfigReader.h>

namespace Lunia {
	namespace XRated {
		namespace Database {

			void LuniaDatabase::Init() {
				//InfoCollections.Npcs.Init(false);
				InfoCollections.Pets.Init(false);
				//InfoCollections.Stages.Init(false);
				//InfoCollections.actions.Init(false);
				//InfoCollections.npcs.Init(false);
				//InfoCollections.stateBundles.Init(false);
				//InfoCollections.itemInfos.Init(false);
			}

			LuniaDatabase& DatabaseInstance() {
				static LuniaDatabase m_Instance;
				return m_Instance;
			}
		}
	}
}