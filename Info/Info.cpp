#include "Info.h"

namespace Lunia {
	namespace XRated {
		namespace Database {

			static LuniaDatabase& GetInstance() {
				static LuniaDatabase m_Instance;
				return m_Instance;
			};
			void LuniaDatabase::Init() {
				//InfoCollections.npcs.Init(false);
			}

			LuniaDatabase& DatabaseInstance() {
				LuniaDatabase& luniaDatabase = GetInstance();
				luniaDatabase.Init();
				return luniaDatabase;
			}
		}
	}
}