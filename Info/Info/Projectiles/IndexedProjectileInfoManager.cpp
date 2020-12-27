#pragma once
#include "IndexedProjectileInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void IndexedProjectileInfoManager::LoadBinaryData()
				{
					manager.Init(L"Database/ProjectileInfos.b");
					manager.Load(L"Database/ProjectileInfosIndex.b");
				}
				void IndexedProjectileInfoManager::MakeIndexAndSave()
				{
					XRated::Database::Info::ProjectileInfo data;
					manager.Init(L"Database/ProjectileInfos.b", data);
					manager.Save(L"Database/ProjectileInfosIndex.b");
				}
				XRated::Database::Info::ProjectileInfo* IndexedProjectileInfoManager::Retrieve(uint32 hash)
				{
					ProjectileMap::iterator ita = Projectiles.find(hash);
					if (ita != Projectiles.end())
						return &ita->second;
					else
					{
						XRated::Database::Info::ProjectileInfo data;
						if (manager.Retrieve(hash, data))
						{
							Projectiles[hash] = data;
							return &Projectiles[hash];
						}
						else
							return NULL;
					}
				}

				XRated::Database::Info::ProjectileInfo* IndexedProjectileInfoManager::Retrieve(const wchar_t* name)
				{
					return Retrieve(StringUtil::Hash(name));
				}

				bool IndexedProjectileInfoManager::Remove(uint32 hash)
				{
					ProjectileMap::iterator it = Projectiles.find(hash);
					if (it != Projectiles.end())
					{
						Projectiles.erase(it);
						return true;
					}

					return false;
				}

				bool IndexedProjectileInfoManager::Remove(const wchar_t* id)
				{
					return Remove(StringUtil::Hash(id));
				}

			}
		}
	}
}