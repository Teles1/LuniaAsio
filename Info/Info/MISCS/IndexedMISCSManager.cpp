#pragma once
#include "IndexedMISCSManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void IndexedMISCInfoManager::LoadBinaryData()
				{
					manager.Init(L"Database/MISCInfos.b");
					manager.Load(L"Database/MISCInfosIndex.b");
				}

				void IndexedMISCInfoManager::MakeIndexAndSave()
				{
					XRated::Database::Info::MiscInfo data;
					manager.Init(L"Database/MISCInfos.b", data);
					manager.Save(L"Database/MISCInfosIndex.b");
				}
				XRated::Database::Info::MiscInfo* IndexedMISCInfoManager::Retrieve(uint32 hash)
				{
					MISCMap::iterator ita = MISCs.find(hash);
					if (ita != MISCs.end())
						return &ita->second;
					else
					{
						XRated::Database::Info::MiscInfo data;
						if (manager.Retrieve(hash, data))
						{
							MISCs[hash] = data;
							return &MISCs[hash];
						}
						else
							return NULL; 
					}
				}

				XRated::Database::Info::MiscInfo* IndexedMISCInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(StringUtil::Hash(id));
				}

				bool IndexedMISCInfoManager::Remove(uint32 hash)
				{
					MISCMap::iterator it = MISCs.find(hash);
					if (it != MISCs.end())
					{
						MISCs.erase(it);
						return true;
					}

					return false;
				}

				bool IndexedMISCInfoManager::Remove(const wchar_t* id)
				{
					return Remove(StringUtil::Hash(id));
				}
			}
		}
	}
}