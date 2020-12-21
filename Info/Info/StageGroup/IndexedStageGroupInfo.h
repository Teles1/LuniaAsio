#pragma once
#include "StageGroupManager.h"
#include <Info/Info/IndexedInfoManager.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class IndexedStageGroupManager : public StageGroupManager {

				public:
					void LoadBinaryData();

					StageGroup* Retrieve(uint32 hash);
					StageGroup* Retrieve(const std::wstring& name);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);
				public:
					void MakeIndexAndSave();
				public:
					void ClearCache() { StageGroups.clear(); }
				private:
					LoaderManager manager;
				};

			}
		}
	}
}