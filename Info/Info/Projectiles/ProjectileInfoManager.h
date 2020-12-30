#pragma once
#include "Projectile.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class ProjectileInfoManager : public Serializer::ISerializable {
				public:
					typedef std::map<uint32, ProjectileInfo, Less<uint32> > ProjectileMap;

				public:
					ProjectileMap Projectiles;

					void LoadProjectileInfo(const wchar_t* xml);

				public:
					ProjectileInfoManager();
					~ProjectileInfoManager();

					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();

					virtual ProjectileInfo* Retrieve(uint32 hash);
					virtual ProjectileInfo* Retrieve(const wchar_t* name);
					void GetList(std::vector<std::wstring>& l);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
