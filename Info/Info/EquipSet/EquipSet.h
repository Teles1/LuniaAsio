#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
#include <Info/Info/Actions/Actions.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct EquipSet
				{
					struct ItemGroup
					{
						std::wstring Id;
						uint32 Hash;
						std::vector< uint32 /*ItemHash*/ > ItemHashs;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);

					};

					struct Reward
					{
						std::wstring Id;
						uint32 Hash;

						std::vector< ActionInfo::State /*StateInfo::Type*/ > States;
						std::wstring EffectName;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);

						std::wstring DescriptionKey;
					};

					struct RewardCondition
					{
						uint8 MinimumRequiredCount;
						XRated::Constants::ClassType ClassType;

						RewardCondition() : MinimumRequiredCount(0), ClassType(XRated::Constants::ClassType::NumberOfClassTypes) {}
						RewardCondition(uint8 minimumRequiredCount, XRated::Constants::ClassType classType)
							: MinimumRequiredCount(minimumRequiredCount), ClassType(classType) {}

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);

						bool operator<(const RewardCondition& rhs) const	// Order by MinimumRequiredCount, ClassType 
						{
							if (MinimumRequiredCount < rhs.MinimumRequiredCount)
								return true;

							if (MinimumRequiredCount == rhs.MinimumRequiredCount
								&& this->ClassType < rhs.ClassType)
								return true;
							// ex. 1-E, 1-A, 2-S, 2-A, 3-A, 4-A, 5-A, 5-E
							return false;
						}

						bool operator==(const RewardCondition& rhs) const
						{
							if (this->MinimumRequiredCount == rhs.MinimumRequiredCount
								&& this->ClassType == rhs.ClassType) return true;
							else return false;
						}

						bool operator!=(const RewardCondition& rhs) const
						{
							return !(*this == rhs);
						}

						bool FindAllRewards(const RewardCondition& rhs) const
						{
							if (this->MinimumRequiredCount >= rhs.MinimumRequiredCount &&
								(this->ClassType == rhs.ClassType || rhs.ClassType == XRated::Constants::ClassType::AnyClassType))
							{
								return true;
							}
							else
							{
								return false;
							}
						}
					};

					std::wstring Id;
					uint32 Hash;
					std::vector< std::wstring/* ItemGroup Id*/ > EquipGroups;
					typedef std::map< RewardCondition, std::wstring/*Reward Id*/> RewardContainerType;
					RewardContainerType Rewards;

					uint32 GetAppropriateRewardHash(const RewardCondition& condition) const;
					std::vector<uint32> GetAvailableRewardHashs(const RewardCondition& condition) const;

					std::vector< uint32 /*ItemHashs*/> Jokers;

					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);


					std::wstring DescriptionKey;
				};
			}
		}
	}
}