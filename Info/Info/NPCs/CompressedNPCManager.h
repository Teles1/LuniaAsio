#pragma once
#include "NPCManager.h"
#include "../CompressedLoader.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class CompressedNPCInfoManager : public NPCInfoManager
				{
				public:
					// allm shits
					//AllM::XRated::Database::Info::CompressedMap<unsigned int,AllM::XRated::Database::Info::NonPlayerInfo,AllM::XRated::Database::Info::Less<unsigned int> >

					typedef std::map<uint32, uint32, Less<uint32> > CompressedNPCManager;
					PositionData<CompressedNPCManager> compressedNpcs;

					Resource::StreamReader compressedNpcCbf;

					void LoadBinaryData();
					void BinaryToXml();
					void GetNPC(const uint32 templateOffset);
					NonPlayerInfo* Retrieve(const uint32 hash);
					NonPlayerInfo* Retrieve(const wchar_t* name); 
					NPCInfoManager::BasicResist::Resist* Retrieve(NonPlayerInfo::Races race);
				};
			}
		}
	}
}