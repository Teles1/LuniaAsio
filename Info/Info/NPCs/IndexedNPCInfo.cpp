#pragma once
#include "../Database.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void IndexedNPCInfoManager::LoadBinaryData()
				{
					loader = CreateLoader(L"Database/NPCInfos.b");
					npcManager = CreateIndexedManagerWithMap(loader);
					com_ptr<IndexedManagerWithElement> basicResistManager = CreateIndexedManagerWithElement(loader, L"basicResist");

					npcManager->Load(L"Database/NPCInfosIndex.b");
					basicResistManager->Load(L"Database/BasicResistIndex.b");
					basicResistManager->Get(basicResist);
				}
				void IndexedNPCInfoManager::MakeIndexAndSave()
				{
					Database::Info::NonPlayerInfo data;
					loader = CreateLoader(L"Database/NPCInfos.b");
					npcManager = CreateIndexedManagerWithMap(loader, data);

					com_ptr<IndexedManagerWithElement> basicResistManager = CreateIndexedManagerWithElement(loader, L"basicResist", basicResist);

					npcManager->Save(L"Database/NPCInfosIndex.b");
					basicResistManager->Save(L"Database/BasicResistIndex.b");
				}
				XRated::Database::Info::NonPlayerInfo* IndexedNPCInfoManager::Retrieve(uint32 hash)
				{
					NPCInfoMap::iterator ita = Npcs.find(hash);
					if (ita != Npcs.end())
						return &ita->second;
					else if (npcManager != NULL)
					{
						XRated::Database::Info::NonPlayerInfo data;
						if (npcManager->Get(hash, data))
						{
							Npcs[hash] = data;
							return &Npcs[hash];
						}
					}
					return NULL;
				}

				XRated::Database::Info::NonPlayerInfo* IndexedNPCInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(StringUtil::Hash(id));
				}

				bool IndexedNPCInfoManager::Remove(uint32 hash)
				{
					NPCInfoMap::iterator it = Npcs.find(hash);
					if (it != Npcs.end())
					{
						Npcs.erase(it);
						return true;
					}

					return false;
				}

				bool IndexedNPCInfoManager::Remove(const wchar_t* id)
				{
					return Remove(StringUtil::Hash(id));
				}
			}
		}
	}
}