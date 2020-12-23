#pragma once
#include "IndexedStageGroupInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void IndexedStageGroupManager::LoadBinaryData()
				{
					manager.Init(L"Database/StageGroups.b");
					manager.Load(L"Database/StageGroupsIndex.b");
				}

				void IndexedStageGroupManager::MakeIndexAndSave()
				{
					XRated::Database::Info::StageGroup data;
					manager.Init(L"Database/StageGroups.b", data);
					manager.Save(L"Database/StageGroupsIndex.b");
				}

				XRated::Database::Info::StageGroup* IndexedStageGroupManager::Retrieve(uint32 hash)
				{
					StageGroupMap::iterator ita = StageGroups.find(hash);
					if (ita != StageGroups.end())
						return &ita->second;
					else
					{
						XRated::Database::Info::StageGroup data;
						if (manager.Retrieve(hash, data))
						{
							StageGroups[hash] = data;
							return &StageGroups[hash];
						}
						else
							return NULL; 
					}
				}

				XRated::Database::Info::StageGroup* IndexedStageGroupManager::Retrieve(const std::wstring& name)
				{
					return Retrieve(StringUtil::Hash(name.c_str()));
				}
				
				bool IndexedStageGroupManager::Remove(uint32 hash)
				{
					StageGroupMap::iterator it = StageGroups.find(hash);
					if (it != StageGroups.end())
					{
						StageGroups.erase(it);
						return true;
					}

					return false;
				}

				bool IndexedStageGroupManager::Remove(const wchar_t* id)
				{
					return Remove(StringUtil::Hash(id));
				}
			}
		}
	}
}