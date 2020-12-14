#pragma once

#include <Core/Core.h>
#include "./Info/Database.h"

namespace Lunia {
	namespace XRated {
		namespace Database {

			class LuniaDatabase {
				public:
					void Init();

					struct Collection {
						Info::IndexedNPCInfoManager npcs;
					};
			};
		}
	}
}