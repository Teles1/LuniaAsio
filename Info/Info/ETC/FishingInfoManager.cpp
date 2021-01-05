#include "FishingInfoManager.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void FishingInfoManager::Init(bool bForceXML)
				{
					if (bForceXML)
					{
						LoadXmlData();
					}
					else
					{
						LoadBinaryData();
					}
				}

				void FishingInfoManager::LoadXmlData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/FishingInfos.xml");
					reader->Read(L"FishingRods", FishingRods);
					reader->Read(L"Baits", Baits);
					reader->Read(L"Fishes", Fishes);

					MakeRareInfoTable();
				}

				void FishingInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/FishingInfos.b");
					reader->Read(L"FishingInfoManager", *this);

					MakeRareInfoTable();
				}

				void FishingInfoManager::MakeRareInfoTable()
				{
					std::map< uint32 /* hash */, FishingRodInfo >::iterator iRod = FishingRods.begin();
					for (; iRod != FishingRods.end(); ++iRod)
					{
						std::map< uint32 /*stageHash*/, FishingRodInfo::RodInfo >::iterator iRodInfo = iRod->second.StageHashIndexRodInfos.begin();
						for (; iRodInfo != iRod->second.StageHashIndexRodInfos.end(); ++iRodInfo)
						{
							for (uint32 i = 0; i < iRodInfo->second.ProbabilityTables.size(); ++i)
							{
								RareInfo[iRodInfo->second.ProbabilityTables[i].ItemHash] = iRodInfo->second.ProbabilityTables[i].Rare;
							}
						}
					}
				}
				void FishingInfoManager::GetRodList(std::vector< uint32 >& rodList)
				{
					rodList.clear();
					std::map< uint32 /* hash */, FishingRodInfo >::iterator it = FishingRods.begin();
					std::map< uint32 /* hash */, FishingRodInfo >::iterator endIt = FishingRods.end();
					for (; it != endIt; ++it)
					{
						rodList.push_back(it->first);
					}
				}
				void FishingInfoManager::GetBaitList(std::vector< uint32 >& baitList)
				{
					baitList.clear();
					std::map< uint32 /* hash */, float >::iterator it = Baits.begin();
					std::map< uint32 /* hash */, float >::iterator endIt = Baits.end();
					for (; it != endIt; ++it)
					{
						baitList.push_back(it->first);
					}
				}

				uint32 FishingInfoManager::RetrieveID(uint32 hash) const
				{
					std::map< uint32, FishingRodInfo >::const_iterator	findIter;

					findIter = FishingRods.find(hash);

					if (findIter != FishingRods.end())
					{
						return findIter->second.Id;
					}

					return 0;
				}

				const FishingRodInfo::RodInfo* FishingInfoManager::Retrieve(uint32 hash, uint32 stageHash) const
				{
					std::map< uint32, FishingRodInfo >::const_iterator	findIter;

					findIter = FishingRods.find(hash);

					if (findIter != FishingRods.end())
					{
						std::map< uint32, FishingRodInfo::RodInfo >::const_iterator iter = findIter->second.StageHashIndexRodInfos.find(stageHash);
						std::map< uint32, FishingRodInfo::RodInfo >::const_iterator defaultIter = findIter->second.StageHashIndexRodInfos.find(FishingRodInfo::DefaultStageHash);

						if (iter != findIter->second.StageHashIndexRodInfos.end())
						{
							return &iter->second;
						}
						else if (defaultIter != findIter->second.StageHashIndexRodInfos.end())
						{
							return &defaultIter->second;
						}
					}

					return NULL;
				}

				const FishingRodInfo::ProbabilityTable* FishingInfoManager::RetrieveFishInfo(uint32 rodHash, uint32 stageHash, uint32 itemHash)
				{
					const FishingRodInfo::RodInfo* rodInfo = Retrieve(rodHash, stageHash);
					if (rodInfo == NULL)
						return NULL;
					std::vector<FishingRodInfo::ProbabilityTable>::const_iterator i = rodInfo->ProbabilityTables.begin();
					for (; i != rodInfo->ProbabilityTables.end(); ++i) {
						if ((*i).ItemHash == itemHash)
							return &(*i);
					}
					return NULL;
				}

				int32 FishingInfoManager::RetrieveRareInfo(uint32 itemHash)
				{
					std::map< uint32 /*item hash*/, int32 /*rare*/ >::iterator iter = RareInfo.find(itemHash);
					if (iter != RareInfo.end())
						return iter->second;

					return -1; //Unable to find item rareinfo
				}

				bool FishingInfoManager::IsFishingRod(uint32 hash) const
				{
					std::map< uint32, FishingRodInfo >::const_iterator	findIter;

					findIter = FishingRods.find(hash);

					if (findIter != FishingRods.end())
						return true;
					else
					{
						return false;
					}
				}

				uint32 FishingInfoManager::GetFishIndex(uint32 hash) const
				{
					uint32 ret = 0;

					for (uint32 i = 0; i < Fishes.size(); ++i)
					{
						if (Fishes[i] == hash)
						{
							ret = i + 1;
							break;
						}
					}

					return ret;
				}

				float FishingInfoManager::RetrieveBaitInfo(uint32 hash) const
				{
					std::map< uint32, float >::const_iterator	findIter;

					findIter = Baits.find(hash);

					if (findIter != Baits.end())
						return findIter->second;
				}

				void FishingInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::FishingInfoManager");
					out.Write(L"FishingRods", FishingRods);
					out.Write(L"Baits", Baits);
					out.Write(L"Fishes", Fishes);
				}

				void FishingInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::xRated::Database::Info::FishingInfoManager");
					in.Read(L"FishingRods", FishingRods);
					in.Read(L"Baits", Baits);
					in.Read(L"Fishes", Fishes);
				}

			}
		}
	}
}
