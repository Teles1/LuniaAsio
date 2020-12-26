#pragma once
#include "StatusInfo.h"
#include "StatusInfoManager.h"
#include <Info/Info/IndexedInfoManager.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class IndexedStatusInfoManager : public XRated::Database::Info::StatusInfoManager {

				public:
					void LoadBinaryData();

					StatusGroup* Retrieve(uint32 hash);
					StatusGroup* Retrieve(const std::wstring& type);
					StatusInfo* Retrieve(int level, const std::wstring& type);
					StatusInfo* Retrieve(int level, uint32 hash);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);
				public:
					void MakeIndexAndSave();
				public:
					void ClearCache() { basicStatMap.clear(); statusTable.clear(); }
				private:
					com_ptr<Loader>			loader;
					com_ptr<IndexedManagerWithMap>	statusManager;
					com_ptr<IndexedManagerWithElement> expManager;
					com_ptr<IndexedManagerWithElement> rebirthAddtionalFactorsManager;
					com_ptr<IndexedManagerWithElement> rebirthAddtionalMajorFactorsManager;
					//LoaderManager manager;
				};
			}
		}
	}
}