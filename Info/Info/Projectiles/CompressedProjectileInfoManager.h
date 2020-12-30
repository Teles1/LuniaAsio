#pragma once
#include "ProjectileInfoManager.h"
#include "../CompressedLoader.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class CompressedProjectileInfoManager : public ProjectileInfoManager, CompressedLoader
				{
				public:
					// allm shits
					//AllM::XRated::Database::Info::CompressedMap<unsigned int,AllM::XRated::Database::Info::NonPlayerInfo,AllM::XRated::Database::Info::Less<unsigned int> >

					typedef std::map<uint32, uint32, Less<uint32> > CompressedProjectileManager;
					PositionData<CompressedProjectileManager> compressedProjectiles;

					Resource::StreamReader compressedProjectileCBF;

					void Init();
					void LoadBinaryData();
					void BinaryToXml();
					void GetProjectile(const uint32 templateOffset);
					ProjectileInfo* Retrieve(const uint32 hash);
					ProjectileInfo* Retrieve(const wchar_t* name);
				};
			}
		}
	}
}