#pragma once
#include "IndexedStatusInfoManager.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void IndexedStatusInfoManager::LoadBinaryData()
				{
					loader = CreateLoader(L"Database/StatusInfos.b");
					statusManager = CreateIndexedManagerWithMap(loader);
					expManager = CreateIndexedManagerWithElement(loader, L"ExpTable");
					com_ptr<IndexedManagerWithElement> rebirthAddtionalFactorsManager = CreateIndexedManagerWithElement(loader, L"RebirthAddtionalFactors");
					com_ptr<IndexedManagerWithElement> rebirthAddtionalMajorFactorsManager = CreateIndexedManagerWithElement(loader, L"RebirthAddtionalMajorFactors");


					statusManager->Load(L"Database/StatusInfosIndex.b");
					expManager->Load(L"Database/ExpInfoIndex.b");
					expManager->Get(expTable);

					rebirthAddtionalFactorsManager->Load(L"Database/RebirthAddtionalFactorsIndex.b");
					rebirthAddtionalFactorsManager->Get(rebirthAddtionalFactors);

					rebirthAddtionalMajorFactorsManager->Load(L"Database/RebirthAddtionalMajorFactorsIndex.b");
					rebirthAddtionalMajorFactorsManager->Get(rebirthAddtionalMajorFactors);
				}

				void IndexedStatusInfoManager::MakeIndexAndSave()
				{
					XRated::Database::Info::BasicStatInfo data;

					loader = CreateLoader(L"Database/StatusInfos.b");
					statusManager = CreateIndexedManagerWithMap(loader, data);
					expManager = CreateIndexedManagerWithElement(loader, L"ExpTable", expTable);

					rebirthAddtionalFactorsManager = CreateIndexedManagerWithElement(loader, L"RebirthAddtionalFactors", rebirthAddtionalFactors);
					rebirthAddtionalFactorsManager->Save(L"Database/RebirthAddtionalFactorsIndex.b");

					rebirthAddtionalMajorFactorsManager = CreateIndexedManagerWithElement(loader, L"RebirthAddtionalMajorFactors", rebirthAddtionalMajorFactors);
					rebirthAddtionalMajorFactorsManager->Save(L"Database/RebirthAddtionalMajorFactorsIndex.b");

					statusManager->Save(L"Database/StatusInfosIndex.b");
					expManager->Save(L"Database/ExpInfoIndex.b");
				}


				StatusInfo* IndexedStatusInfoManager::Retrieve(int level, uint32 hash)
				{
					StatusTable::iterator ita = statusTable.find(hash);
					if (ita != statusTable.end())
						return ((*ita).second).statusList[level - 1];
					else
					{
						XRated::Database::Info::BasicStatInfo data;
						if (statusManager->Get(hash, data))
						{
							LoadStatusTable(data.name, data);
							return (statusTable[hash].statusList[level - 1]);
						}
						else
							return NULL;
					}
				}

				StatusInfo* IndexedStatusInfoManager::Retrieve(int level, const std::wstring& type)
				{
					return Retrieve(level, StringUtil::Hash(type.c_str()));
				}


				IndexedStatusInfoManager::StatusGroup* IndexedStatusInfoManager::Retrieve(uint32 hash)
				{
					StatusTable::iterator ita = statusTable.find(hash);
					if (ita != statusTable.end())
						return &ita->second;
					else
					{
						XRated::Database::Info::BasicStatInfo data;
						if (statusManager->Get(hash, data))
						{
							LoadStatusTable(data.name, data);
							return &statusTable[hash];
						}
						else
							return NULL;
					}
				}

				IndexedStatusInfoManager::StatusGroup* IndexedStatusInfoManager::Retrieve(const std::wstring& type)
				{
					return Retrieve(StringUtil::Hash(type.c_str()));
				}

				bool IndexedStatusInfoManager::Remove(uint32 hash)
				{
					BasicStatMap::iterator it = basicStatMap.find(hash);
					if (it != basicStatMap.end())
					{
						basicStatMap.erase(it);
						return true;
					}

					StatusTable::iterator ita = statusTable.find(hash);
					if (ita != statusTable.end())
					{
						statusTable.erase(ita);
						return true;
					}

					return false;
				}

				bool IndexedStatusInfoManager::Remove(const wchar_t* id)
				{
					return Remove(StringUtil::Hash(id));
				}
			}
		}
	}
}