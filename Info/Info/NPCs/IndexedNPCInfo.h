#pragma once
#include <Info/Info/IndexedInfoManager.h>
#include "NPCManager.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class IndexedNPCInfoManager : public NPCInfoManager
				{
				public: // i perfer this style ( putting public members on top	 )
					void LoadBinaryData();

					NonPlayerInfo* Retrieve(uint32 hash);
					NonPlayerInfo* Retrieve(const wchar_t* id);
					inline BasicResist::Resist* Retrieve(Database::Info::NonPlayerInfo::Races race) { return NPCInfoManager::Retrieve(race); }

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);

				private:
					void MakeIndexAndSave();
				public:
					inline void ClearCache() { Npcs.clear(); }
				private:
					com_ptr<Loader>			loader;
					com_ptr<IndexedManagerWithMap>	npcManager;
				};
			}
		}
	}
}