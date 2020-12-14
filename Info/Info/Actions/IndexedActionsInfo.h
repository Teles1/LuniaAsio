#pragma once
#include "../Database.h"
#include "ActionsManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class ActionLoaderManager : public LoaderManager
				{
					// make added by kim. template hash key to string table.
					std::map<uint32, std::wstring>	actionTemplateMap;
				public:
					void LoadTemplateTable(const std::wstring& file);
					void SaveTemplateTable(const std::wstring& file);
				public:
					const wchar_t* GetTemplateName(uint32 key);

					void Init(const String& fileName);
					template <typename T> void Init(const String& filename, T& data);
				};

				class IndexedActionInfoManager : public XRated::Database::Info::ActionInfoManager {
				public:
					void LoadBinaryData();

					ActionInfo* Retrieve(const wchar_t* templateName, const wchar_t* actionName);
					ActionInfo* Retrieve(uint32 templateName, uint32 actionName);
					ActionInfo* Retrieve(XRated::Constants::ClassType classType, const wchar_t* actionName);
					ActionInfo* Retrieve(XRated::Constants::ClassType classType, uint32 actionName);
					const std::vector<std::wstring>* RetrieveActionList(const wchar_t* name, const wchar_t* templateName);
					Actions& Retrieve(const wchar_t* templateName);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);
				public:
					void ClearCache() { actionMap.clear(); }
				public:
					void MakeIndexAndSave();

				private:
					ActionLoaderManager manager;
				};

			}
		}
	}
}