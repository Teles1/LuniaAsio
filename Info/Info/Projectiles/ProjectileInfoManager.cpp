#pragma once
#include "ProjectileInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				ProjectileInfoManager::ProjectileInfoManager()
				{
				}

				ProjectileInfoManager::~ProjectileInfoManager()
				{
				}

				void ProjectileInfoManager::Init(bool bForceXmlData)
				{
					Projectiles.clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						std::wstring fullLoading;
						LoadBinaryData();
					}
				}

				void ProjectileInfoManager::LoadXmlData()
				{
					std::vector<std::wstring> projectileList;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Projectiles/ProjectileList.xml");
					reader->Read(L"ProjectileList", projectileList);

					std::vector<std::wstring>::iterator itr, end;
					for (itr = projectileList.begin(), end = projectileList.end(); itr != end; ++itr)
						LoadProjectileInfo((*itr).c_str());

					//Resource::SerializerStreamReader reader = AllM::Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Projectile.xml");
					//reader->Read(L"Projectiles", Projectiles);
				}

				void ProjectileInfoManager::LoadProjectileInfo(const wchar_t* xml)
				{
					ProjectileMap temp;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"Projectiles", temp);

					Projectiles.insert(temp.begin(), temp.end());
				}

				void ProjectileInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/ProjectileInfos.b");
					reader->Read(L"ProjectileInfoManager", *this);
				}

				ProjectileInfo* ProjectileInfoManager::Retrieve(uint32 hash)
				{
					ProjectileMap::iterator i = Projectiles.find(hash);
					if (i != Projectiles.end())
						return &(*i).second;
					return NULL;
				}

				ProjectileInfo* ProjectileInfoManager::Retrieve(const wchar_t* name)
				{
					return Retrieve(StringUtil::Hash(name));
				}

				void ProjectileInfoManager::GetList(std::vector<std::wstring>& l)
				{
					ProjectileMap::iterator i = Projectiles.begin();
					while (i != Projectiles.end()) {
						l.push_back((*i).second.Name);
						++i;
					}
				}

				void ProjectileInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ProjectileInfoManager");
					out.Write(L"Projectiles", Projectiles);
				}

				void ProjectileInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ProjectileInfoManager");
					in.Read(L"Projectiles", Projectiles);
				}
			}
		}
	}
}
