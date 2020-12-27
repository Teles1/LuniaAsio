#pragma once
#include "EventNonPlayerItemList.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void NpcDropEventItems::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::NpcDropEventItems");
					out.Write(L"NpcItems", NpcItems);
					out.Write(L"StageItems", StageItems);
					out.Write(L"AnyWhereItems", AnyWhereItems);
				}

				void NpcDropEventItems::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::NpcDropEventItems");
					in.Read(L"NpcItems", NpcItems);
					in.Read(L"StageItems", StageItems);
					in.Read(L"AnyWhereItems", AnyWhereItems);

					ValidEvents();
				}

				void NpcDropEventItems::ValidEvents()
				{
					{
						float totalProbability = 0.f;
						std::map<uint32/*NpcHash*/, std::vector<NonPlayerInfo::Item> >::const_iterator iter = NpcItems.begin();
						while (iter != NpcItems.end())
						{
							totalProbability = 0.f;

							std::vector<NonPlayerInfo::Item>::const_iterator itemIter = iter->second.begin();
							while (itemIter != iter->second.end())
							{
								totalProbability += itemIter->Probability;
								++itemIter;
							}

							if (totalProbability > 0.1f)
							{
								Logger::GetInstance().Exception(L"NpcDropEventItems Probality over 10% : npcHash={0}", iter->first);
							}

							++iter;
						}
					}

					{
						float totalProbability = 0.f;
						std::map<XRated::StageLocation, std::vector<NonPlayerInfo::Item> >::const_iterator iter = StageItems.begin();
						while (iter != StageItems.end())
						{
							totalProbability = 0.f;

							std::vector<NonPlayerInfo::Item>::const_iterator itemIter = iter->second.begin();
							while (itemIter != iter->second.end())
							{
								totalProbability += itemIter->Probability;
								++itemIter;
							}

							if (totalProbability > 0.1f)
							{
								Logger::GetInstance().Exception(L"NpcDropEventItems Probality over 10% : stageGroupHash={0}, accessLevel={1}", iter->first.StageGroupHash, iter->first.Level);
							}

							++iter;
						}
					}

					{
						float totalProbability = 0.f;
						std::vector<NonPlayerInfo::Item>::const_iterator iter = AnyWhereItems.begin();
						while (iter != AnyWhereItems.end())
						{
							totalProbability += iter->Probability;
							++iter;
						}

						if (totalProbability > 0.1f)
						{
							Logger::GetInstance().Exception(L"NpcDropEventItems AnyWhereItems Probality over 10%");
						}
					}
				}
			}
		}
	}
}