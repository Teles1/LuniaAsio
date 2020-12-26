#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct GeneralInfo : public Serializer::ISerializable {
					double PhysicalCri, MagicCri;
					int MaxNPCCnt; 

					struct CharacterInfo : public Serializer::ISerializable {
						float Radius;
						float MaxSp;
						float RegenHp;
						float RegenMp;
						float RegenSp;
						std::wstring Resource;

					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					std::vector< CharacterInfo > CharacterInfos;

					std::vector< std::vector<int> > PoolSize; // Object pool

					int MultiHitTimeInMSec;
					int MultiHitCount;
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
