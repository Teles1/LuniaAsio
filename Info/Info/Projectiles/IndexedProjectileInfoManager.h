#pragma once
#include "ProjectileInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class IndexedProjectileInfoManager : public XRated::Database::Info::ProjectileInfoManager {

				public:
					void LoadBinaryData();

					ProjectileInfo* Retrieve(uint32 hash);
					ProjectileInfo* Retrieve(const wchar_t* name);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);
				public:
					void MakeIndexAndSave();
				public:
					void ClearCache() { Projectiles.clear(); }
				private:
					LoaderManager manager;
				};
			}
		}
	}
}