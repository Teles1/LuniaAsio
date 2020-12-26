#pragma once
#include "EquipSet.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class EquipSetManager
				{
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);

					void Init(bool bForceXml);

					void LoadXmlData();
					void LoadBinaryData();

					const Info::EquipSet::ItemGroup* GetEquipItemGroup(const uint32 itemHash);
					const Info::EquipSet::ItemGroup* GetEquipItemGroup(const std::wstring itemId);
					const Info::EquipSet* GetEquipSet(const uint32 itemGroupHash); 
					const Info::EquipSet* GetEquipSet(const std::wstring itemGroupId);
					const Info::EquipSet::Reward* GetReward(const uint32 rewardHash);
					const Info::EquipSet::Reward* GetReward(const std::wstring rewardId);
					std::vector<const Info::EquipSet*> GetJokerEquips(const uint32 jokerHash);
					std::vector<const Info::EquipSet*> GetJokerEquips(const std::wstring jokerId);

					void GetReward(std::vector<const Info::EquipSet::Reward*>& result, const std::vector<uint32>& equips, XRated::Constants::ClassType myClass);	

					struct RewardInfo
					{
						uint32 Count;
						const Info::EquipSet* EquipSet;
						std::vector<const Info::EquipSet::Reward*> Rewards;
						std::set< uint32 /*itemHash*/ > Items;
						std::set< uint32 /*joker itemhash*/ > JokerItems;
					};
					void GetRewardEx(std::vector<RewardInfo>& result, const std::vector<uint32>& equips, XRated::Constants::ClassType myClass);	

					typedef  std::map< uint32, Info::EquipSet, Less<uint32> >  EquipSetMap;
					typedef std::map< uint32, Info::EquipSet::Reward, Less<uint32> > EquipSetRewardMap;
					typedef std::map< uint32, Info::EquipSet::ItemGroup, Less<uint32> > EquipSetItemGroupMap;

					typedef std::map< uint32/*ItemHash*/, const Info::EquipSet::ItemGroup*, Less<uint32> > ReversedEquipSetItemGroupMap;
					typedef std::map< uint32/*ItemGroupId*/, const Info::EquipSet*, Less<uint32> > ReversedEquipSetMap;
					typedef std::map< uint32/*JokerItemHash*/, std::set<const Info::EquipSet*, Less<const Info::EquipSet*> >, Less<uint32> > ReversedJokerEquipSetMap;

				protected:
					EquipSetMap equipSets;
					EquipSetRewardMap rewards;
					EquipSetItemGroupMap itemGroups;

					//typedef std::map< uint32/*ItemHash*/ , const Info::EquipSet*, Less<uint32> > ReversedItemToEquipSetMap;
					//ReversedItemToEquipSetMap reversedItemToEquipSetIndexer;

					ReversedEquipSetItemGroupMap reversedItemGroupIndexer;
					ReversedEquipSetMap reversedEquipSetIndexer;
					ReversedJokerEquipSetMap reversedJokerEquipSetIndexer;

					void Validate();
					void PostLoadingProcess()
					{
						//Logger::GetInstance().Warn(L"EquipSetManager::PostLoadingProcess()");
						MakeReversedItemGroupIndexer();
						MakeReversedEquipSetIndexer();
						MakeReversedRewardIndexer();
					}
					virtual void MakeReversedItemGroupIndexer();
					virtual void MakeReversedEquipSetIndexer();
					virtual void MakeReversedRewardIndexer();

					/* fileloading */
					void Clear();
					void LoadItemGroupsXml(const wchar_t* filename);
					void LoadRewardsXml(const wchar_t* filename);
					void LoadEquipSetXml(const wchar_t* filename);
				};
			}
		}
	}
}