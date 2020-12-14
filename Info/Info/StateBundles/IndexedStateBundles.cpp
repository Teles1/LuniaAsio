#pragma once
#include "IndexedStateBundles.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void IndexedStateBundleInfoManager::LoadBinaryData()
				{
					manager.Init(L"Database/StateBundleInfos.b");
					manager.Load(L"Database/StateBundleInfosIndex.b");
				}

				void IndexedStateBundleInfoManager::MakeIndexAndSave()
				{
					XRated::Database::Info::StateBundleInfo data;
					manager.Init(L"Database/StateBundleInfos.b", data);
					manager.Save(L"Database/StateBundleInfosIndex.b");
				}

				XRated::Database::Info::StateBundleInfo* IndexedStateBundleInfoManager::Retrieve(uint32 hash)
				{
					StateBundles::iterator ita = stateBundles.find(hash);
					if (ita != stateBundles.end())
						return &ita->second;
					else
					{
						XRated::Database::Info::StateBundleInfo data;
						if (manager.Retrieve(hash, data))
						{
							stateBundles[hash] = data;
							return &stateBundles[hash];
						}
						else
							return NULL;
					}
				}

				XRated::Database::Info::StateBundleInfo* IndexedStateBundleInfoManager::Retrieve(const std::wstring& name)
				{
					return Retrieve(StringUtil::Hash(name.c_str()));
				}

				bool IndexedStateBundleInfoManager::Remove(uint32 hash)
				{
					StateBundles::iterator it = stateBundles.find(hash);
					if (it != stateBundles.end())
					{
						stateBundles.erase(it);
						return true;
					}

					return false;
				}

				bool IndexedStateBundleInfoManager::Remove(const wchar_t* id)
				{
					return Remove(StringUtil::Hash(id));
				}
			}
		}
	}
}