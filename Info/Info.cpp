#include "Info.h"

namespace Lunia {
	namespace XRated {
		namespace Database {

			static LuniaDatabase& GetInstance() {
				static LuniaDatabase m_Instance;
				return m_Instance;
			};
			void LuniaDatabase::Init() {
				InfoCollections.itemInfos.Init(false);
				//InfoCollections.actions.Init(false);
				//InfoCollections.npcs.Init(false);
				//InfoCollections.stateBundles.Init(false);
				//InfoCollections.itemInfos.Init(false);
			}

			LuniaDatabase& DatabaseInstance() {
				LuniaDatabase& luniaDatabase = GetInstance();
				luniaDatabase.Init();
				return luniaDatabase;
			}
		}
	}
}