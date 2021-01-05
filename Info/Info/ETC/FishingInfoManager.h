#pragma once
#include "FishingRodInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class FishingInfoManager : public Serializer::ISerializable
				{
				protected:
					std::map< uint32 /* hash */, FishingRodInfo >				FishingRods;
					std::map< uint32 /* hash */, float /*Interval*/ >			Baits;
					std::map< uint32 /*item hash*/, int32 /*rare*/ >			RareInfo; //Fill after loading xml/binary data.

					std::vector< uint32 /* hash */ >							Fishes;

					void MakeRareInfoTable();

				public:
					void Init(bool bForceXML);
					void LoadXmlData();
					virtual void LoadBinaryData();

					uint32 RetrieveID(uint32 hash) const;
					const FishingRodInfo::RodInfo* Retrieve(uint32 hash, uint32 stageHash) const;
					const FishingRodInfo::ProbabilityTable* RetrieveFishInfo(uint32 rodHash, uint32 stageHash, uint32 itemHash);
					int32 RetrieveRareInfo(uint32 itemHash);
					bool IsFishingRod(uint32 hash) const;
					uint32 GetFishIndex(uint32 hash) const;

					float RetrieveBaitInfo(uint32 hash) const;
					void GetRodList(std::vector< uint32 >& rodList);
					void GetBaitList(std::vector< uint32 >& BaitList);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};

			}
		}
	}
}