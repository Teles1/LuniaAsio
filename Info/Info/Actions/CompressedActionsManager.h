#pragma once
#include "ActionsManager.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class CompressedActionInfoManager : public XRated::Database::Info::ActionInfoManager
				{
				public:
					void Init(bool forceXml = false);
					void LoadBinaryData();
					void LoadCBFInData();
					void SaveXmlData();
					void GetData(ActionInfoManager::Actions& actionMap);
					void GetData(Resource::StreamReader& reader, ActionInfoManager::Actions& actionMap);
					ActionInfoManager::Actions& Retrieve(const wchar_t* templateName);
					inline void ClearCache() { actionMap.clear(); }
				};
			}
		}
	}
}
