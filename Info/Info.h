#pragma once

#include <Core/Core.h>
#include "./Info/Database.h"
#include "Info/GlobalInfo.h"
#include "Info/NPCs/IndexedNPCInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {

			class LuniaDatabase {
				public:


					void Init();

					struct Collection {
						Info::IndexedNPCInfoManager npcs;
					} InfoCollections;
			};

			LuniaDatabase& DatabaseInstance();
		}
	}
}