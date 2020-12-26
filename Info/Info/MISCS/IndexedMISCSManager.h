#pragma once
#include "MISCSManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class IndexedMISCInfoManager : public XRated::Database::Info::MISCInfoManager {

				public:
					void LoadBinaryData();

					MiscInfo* Retrieve(const wchar_t* id);
					MiscInfo* Retrieve(uint32 hash);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);

				public:
					void MakeIndexAndSave();
				public:
					void ClearCache() { MISCs.clear(); }
				private:
					LoaderManager manager;
				};

			}
		}
	}
}