#pragma once
#include "EquipSet.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void EquipSet::ItemGroup::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::EquipSet::ItemGroup");
					out.Write(L"Id", Id);
					out.Write(L"ItemHashs", ItemHashs);
				}

				void EquipSet::ItemGroup::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::EquipSet::ItemGroup");
					in.Read(L"Id", Id);		Hash = StringUtil::Hash(Id.c_str());
					in.Read(L"ItemHashs", ItemHashs);
				}

				void EquipSet::Reward::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::EquipSet::Reward");
					out.Write(L"Id", Id);
					out.Write(L"DescriptionKey", DescriptionKey);
					out.Write(L"States", States);
					out.Write(L"EffectName", EffectName);
				}

				void EquipSet::Reward::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::EquipSet::Reward");
					in.Read(L"Id", Id);		Hash = StringUtil::Hash(Id.c_str());
					in.Read(L"DescriptionKey", DescriptionKey);
					in.Read(L"States", States);
					in.Read(L"EffectName", EffectName, std::wstring(L""));
				}

				void EquipSet::RewardCondition::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::EquipSet::RewardCondition");
					out.Write(L"MinimumRequiredCount", MinimumRequiredCount);
					out.WriteEnum(L"ClassType", ClassType);
				}

				void EquipSet::RewardCondition::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::EquipSet::RewardCondition");
					in.Read(L"MinimumRequiredCount", MinimumRequiredCount);
					in.ReadEnum(L"ClassType", ClassType);
				}

				void EquipSet::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::EquipSet");
					out.Write(L"Id", Id);
					out.Write(L"DescriptionKey", DescriptionKey);
					out.Write(L"EquipGroups", EquipGroups);
					out.Write(L"Rewards", Rewards);
					out.Write(L"Jokers", Jokers);

				}

				void EquipSet::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::EquipSet");
					in.Read(L"Id", Id);		Hash = StringUtil::Hash(Id.c_str());
					in.Read(L"DescriptionKey", DescriptionKey);
					in.Read(L"EquipGroups", EquipGroups);
					in.Read(L"Rewards", Rewards);
					in.Read(L"Jokers", Jokers);
				}

				uint32 EquipSet::GetAppropriateRewardHash(const RewardCondition& condition) const
				{
					RewardContainerType::const_iterator it;
					it = Rewards.find(condition);
					if (Rewards.end() != it)
					{
						return StringUtil::Hash(it->second.c_str());
					}

					it = Rewards.find(RewardCondition(condition.MinimumRequiredCount, XRated::Constants::ClassType::AnyClassType));
					if (Rewards.end() != it)
					{
						return StringUtil::Hash(it->second.c_str());
					}

					return NULL;
				}
				std::vector<uint32> EquipSet::GetAvailableRewardHashs(const RewardCondition& condition) const
				{
					std::vector< RewardContainerType::const_iterator > found;
					RewardContainerType::const_iterator it = Rewards.begin();
					for (; it != Rewards.end() && it->first.MinimumRequiredCount <= condition.MinimumRequiredCount; ++it)
					{
						if (it->first.ClassType == condition.ClassType)
						{
							found.push_back(it); //Exactly Same!
						}
						else if (it->first.ClassType == XRated::Constants::ClassType::AnyClassType)
						{
							if (found.size() == 0
								|| (found.size() > 0 && found.back()->first.MinimumRequiredCount !=/*it means < */ it->first.MinimumRequiredCount)
								)
								// at least AnyClass without same count and other classType
								found.push_back(it);
						}
					}

					std::vector<uint32> result;
					for (size_t i = 0; i < found.size(); ++i)
					{
						result.push_back(StringUtil::Hash(found[i]->second.c_str()));
					}
					return result;
				}

			}
		}
	}
}