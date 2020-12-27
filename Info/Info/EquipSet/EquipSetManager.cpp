#pragma once
#include "EquipSetManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void EquipSetManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::EquipSetManager");
					out.Write(L"EquipSets", equipSets);
					out.Write(L"Rewards", rewards);
					out.Write(L"ItemGroups", itemGroups);
				}
				void EquipSetManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::EquipSetManager");
					in.Read(L"EquipSets", equipSets);
					in.Read(L"Rewards", rewards);
					in.Read(L"ItemGroups", itemGroups);

					PostLoadingProcess();
				}

				void EquipSetManager::Init(bool bForceXmlData)
				{
					if (bForceXmlData) //Xml loading
						LoadXmlData();
					else //Binary loading
						LoadBinaryData();

				}

				void EquipSetManager::LoadXmlData()
				{
					Clear();

					LoadItemGroupsXml(L"Database/Items/EquipSet/ItemGroups.xml");
					LoadRewardsXml(L"Database/Items/EquipSet/Rewards.xml");
					LoadEquipSetXml(L"Database/Items/EquipSet/EquipSet.xml");

					Validate();

					PostLoadingProcess();
				}

				void EquipSetManager::LoadBinaryData()
				{
					Clear();
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/EquipSet.b");
					reader->Read(L"EquipSetManager", *this);
				}

				void EquipSetManager::Clear()
				{
					equipSets.clear();
					rewards.clear();
					itemGroups.clear();
					reversedItemGroupIndexer.clear();
					reversedEquipSetIndexer.clear();
				}
				void EquipSetManager::LoadItemGroupsXml(const wchar_t* filename)
				{
					std::vector<Info::EquipSet::ItemGroup> tempItemGroups;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader->Read(L"ItemGroups", tempItemGroups);
					for (size_t i = 0; i < tempItemGroups.size(); ++i)
					{
						itemGroups[tempItemGroups[i].Hash] = tempItemGroups[i];
					}
				}
				void EquipSetManager::LoadRewardsXml(const wchar_t* filename)
				{
					std::vector<Info::EquipSet::Reward> tempRewards;
					Resource::SerializerStreamReader reader2 = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader2->Read(L"Rewards", tempRewards);
					for (size_t i = 0; i < tempRewards.size(); ++i)
					{
						rewards[tempRewards[i].Hash] = tempRewards[i];
					}
				}
				void EquipSetManager::LoadEquipSetXml(const wchar_t* filename)
				{
					std::vector<Info::EquipSet> tempEquipSets;
					Resource::SerializerStreamReader reader3 = Resource::ResourceSystemInstance().CreateDefaultDeserializer(filename);
					reader3->Read(L"EquipSets", tempEquipSets);
					for (size_t i = 0; i < tempEquipSets.size(); ++i)
					{
						equipSets[tempEquipSets[i].Hash] = tempEquipSets[i];
					}
				}

				const Info::EquipSet::ItemGroup* EquipSetManager::GetEquipItemGroup(const uint32 hash)
				{
					ReversedEquipSetItemGroupMap::const_iterator i = reversedItemGroupIndexer.find(hash);
					if (i != reversedItemGroupIndexer.end())
						return i->second;
					return NULL;
				}

				const Info::EquipSet::ItemGroup* EquipSetManager::GetEquipItemGroup(const std::wstring Id)
				{
					return EquipSetManager::GetEquipItemGroup(StringUtil::Hash(Id.c_str()));
				}

				const Info::EquipSet* EquipSetManager::GetEquipSet(const uint32 hash)
				{
					ReversedEquipSetMap::const_iterator i = reversedEquipSetIndexer.find(hash);
					if (i != reversedEquipSetIndexer.end())
						return i->second;
					return NULL;
				}

				const Info::EquipSet* EquipSetManager::GetEquipSet(const std::wstring Id)
				{
					return EquipSetManager::GetEquipSet(StringUtil::Hash(Id.c_str()));
				}

				struct FoundInfo
				{
					FoundInfo() :Count(0) {}
					uint8 Count;
					std::vector<uint32> Items;
				};

				void EquipSetManager::GetReward(std::vector<const Info::EquipSet::Reward*>& result, const std::vector<uint32>& equips, XRated::Constants::ClassType myClass)
				{
					std::map<const Info::EquipSet::ItemGroup*, FoundInfo> foundGroups;
					for (size_t i = 0; i < equips.size(); ++i)
					{
						ReversedEquipSetItemGroupMap::const_iterator it = reversedItemGroupIndexer.find(equips[i]);

						if (it != reversedItemGroupIndexer.end())
						{
							FoundInfo& foundInfo = foundGroups[it->second];
							++foundInfo.Count;
							foundInfo.Items.push_back(equips[i]);
						}
					}


					result.clear();
					std::map<const Info::EquipSet::ItemGroup*, FoundInfo >::const_iterator it = foundGroups.begin();
					for (; it != foundGroups.end(); ++it)
					{
						ReversedEquipSetMap::const_iterator eIt = reversedEquipSetIndexer.find(it->first->Hash);
						if (eIt == reversedEquipSetIndexer.end())
							continue;
						const Info::EquipSet& equipSet = *(eIt->second);

						EquipSet::RewardCondition condition;
						condition.ClassType = myClass;
						condition.MinimumRequiredCount = it->second.Count;

						for (size_t i = 0; i < equips.size(); ++i)
						{
							if (std::find(equipSet.Jokers.begin(), equipSet.Jokers.end(), equips[i]) != equipSet.Jokers.end())
							{
								++condition.MinimumRequiredCount;
							}
						}

						std::vector<uint32> foundRewards = equipSet.GetAvailableRewardHashs(condition);

						for (size_t i = 0; i < foundRewards.size(); ++i)
						{
							EquipSetRewardMap::const_iterator it = rewards.find(foundRewards[i]);
							if (it == rewards.end()) {}
							//Logger::GetInstance().Exception(L"Cannot Found Reward[{0}]", foundRewards[i]);

							result.push_back(&it->second);
						}
					}
				}

				void EquipSetManager::GetRewardEx(std::vector<RewardInfo>& result, const std::vector<uint32>& equips, XRated::Constants::ClassType myClass)	//���� �����ϰ� �ִ� ��� �Ѱ��ش�
				{
					std::map<const Info::EquipSet::ItemGroup*, FoundInfo> foundGroups;
					for (size_t i = 0; i < equips.size(); ++i)
					{
						ReversedEquipSetItemGroupMap::const_iterator it = reversedItemGroupIndexer.find(equips[i]);

						if (it != reversedItemGroupIndexer.end())
						{
							FoundInfo& foundInfo = foundGroups[it->second];
							++foundInfo.Count;
							foundInfo.Items.push_back(equips[i]);
						}
					}


					result.clear();
					std::map<const Info::EquipSet::ItemGroup*, FoundInfo >::const_iterator it = foundGroups.begin();
					for (; it != foundGroups.end(); ++it)
					{
						ReversedEquipSetMap::const_iterator eIt = reversedEquipSetIndexer.find(it->first->Hash);
						if (eIt == reversedEquipSetIndexer.end())
							continue;
						const Info::EquipSet& equipSet = *(eIt->second);

						EquipSet::RewardCondition condition;
						condition.ClassType = myClass;
						condition.MinimumRequiredCount = it->second.Count;

						result.push_back(RewardInfo());
						result.back().EquipSet = &equipSet;
						result.back().Items.insert(it->second.Items.begin(), it->second.Items.end());



						for (size_t i = 0; i < equips.size(); ++i)
						{
							if (std::find(equipSet.Jokers.begin(), equipSet.Jokers.end(), equips[i]) != equipSet.Jokers.end())
							{
								result.back().JokerItems.insert(equips[i]);
								++condition.MinimumRequiredCount;
							}
						}
						result.back().Count = condition.MinimumRequiredCount;

						std::vector<uint32> foundRewards = equipSet.GetAvailableRewardHashs(condition);

						for (size_t i = 0; i < foundRewards.size(); ++i)
						{
							EquipSetRewardMap::const_iterator it = rewards.find(foundRewards[i]);
							if (it == rewards.end()) {}
							//Logger::GetInstance().Exception(L"Cannot Found Reward[{0}]", foundRewards[i]);

							result.back().Rewards.push_back(&it->second);
						}
					}
				}

				const Info::EquipSet::Reward* EquipSetManager::GetReward(const uint32 rewardHash)
				{
					EquipSetRewardMap::const_iterator itr = rewards.find(rewardHash);
					if (itr != rewards.end())
						return &itr->second;
					return NULL;
				}

				const Info::EquipSet::Reward* EquipSetManager::GetReward(const std::wstring rewardId) {
					return EquipSetManager::GetReward(StringUtil::Hash(rewardId.c_str()));
				}

				std::vector<const Info::EquipSet*> EquipSetManager::GetJokerEquips(const uint32 hash) {
					std::vector<const Info::EquipSet*> result;
					ReversedJokerEquipSetMap::const_iterator i = reversedJokerEquipSetIndexer.find(hash);
					if (i != reversedJokerEquipSetIndexer.end())
					{
						result.insert(result.begin(), i->second.begin(), i->second.end());
					}
					return result;
				}

				std::vector<const Info::EquipSet*> EquipSetManager::GetJokerEquips(const std::wstring jokerId) {
					return EquipSetManager::GetJokerEquips(StringUtil::Hash(jokerId.c_str()));
				}

				void EquipSetManager::Validate() {
					//Todo 
				}
				void EquipSetManager::MakeReversedItemGroupIndexer() {
					reversedItemGroupIndexer.clear();
					EquipSetItemGroupMap::const_iterator it = itemGroups.begin();
					for (; it != itemGroups.end(); ++it)
					{
						for (size_t j = 0; j < it->second.ItemHashs.size(); ++j)
						{
							reversedItemGroupIndexer[it->second.ItemHashs[j]] = &(it->second);
						}
					}
				}

				void EquipSetManager::MakeReversedEquipSetIndexer() {
					reversedEquipSetIndexer.clear();
					EquipSetMap::const_iterator  it = equipSets.begin();
					for (; it != equipSets.end(); ++it)
					{
						std::vector<std::wstring>::const_iterator it2 = it->second.EquipGroups.begin();
						for (; it2 != it->second.EquipGroups.end(); ++it2)
						{
							reversedEquipSetIndexer[StringUtil::Hash(it2->c_str())] = &(it->second);
						}
					}
				}

				void EquipSetManager::MakeReversedRewardIndexer() {
					reversedJokerEquipSetIndexer.clear();
					for (auto& x: equipSets) {
						for (auto& y: x.second.Jokers) {
							ReversedJokerEquipSetMap::mapped_type& v = reversedJokerEquipSetIndexer[y];
							v.insert(&(x.second));
						}
					}
				}
			}
		}
	}
}