#pragma once
#include "../Database.h"
#include "StateBundles.h"
#include "StateBundlesManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class IndexedStateBundleInfoManager : public XRated::Database::Info::StateBundleInfoManager {

				public:
					void LoadBinaryData();

					StateBundleInfo* Retrieve(uint32 name);
					StateBundleInfo* Retrieve(const std::wstring& name);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);
				public:
					void MakeIndexAndSave();
				public:
					void ClearCache() { stateBundles.clear(); }
				private:
					LoaderManager manager;
				};
			}
		}
	}
}