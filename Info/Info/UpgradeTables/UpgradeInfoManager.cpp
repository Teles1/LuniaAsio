#pragma once
#include "UpgradeInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				bool UpgradeInfoManager::IsNewPotionItem(const Database::Info::ItemInfo* itemInfo)
				{
					if (itemInfo == NULL)
						return false;

					ActionInfo::StateList::const_iterator i
						= std::find_if(itemInfo->States.begin(), itemInfo->States.end(), ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());
					if (i == itemInfo->States.end())
						return false;

					std::map<String, String>::const_iterator keyValue = i->params.find(L"UpgradePotion");
					if (keyValue == i->params.end())
						return false;

					keyValue = i->params.find(L"Table");
					if (keyValue == i->params.end())
						return false;

					NewUpgradeTableMap::const_iterator it = NewUpgradeTables.find(keyValue->second);
					return (it != NewUpgradeTables.end());
				}

				bool UpgradeInfoManager::IsPotionItem(const Database::Info::ItemInfo* itemInfo)
				{
					if (itemInfo == NULL)
						return false;

					ActionInfo::StateList::const_iterator i
						= std::find_if(itemInfo->States.begin(), itemInfo->States.end(), ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());
					if (i == itemInfo->States.end())
						return false;

					std::map<String, String>::const_iterator keyValue = i->params.find(L"Upgrade");
					if (keyValue == i->params.end())
						return false;

					return true;
				}

				bool UpgradeInfoManager::IsTargetItem(const Database::Info::ItemInfo* potion, const Database::Info::ItemInfo* target)
				{
					return (Retrieve(potion, target) != NULL);
				}

				void UpgradeInfoManager::GetTargetList(const Database::Info::ItemInfo* potionItem, std::list<uint32/*target Hash*/>& targetList)
				{
					if (potionItem == NULL)
						return;

					ActionInfo::StateList::const_iterator i = std::find_if(potionItem->States.begin(), potionItem->States.end(), ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());
					if (i == potionItem->States.end())
						return;

					std::map<String, String>::const_iterator keyValue = i->params.find(L"Table");
					if (keyValue == i->params.end())
						return;

					targetList.clear();
					UpgradeTableMap::const_iterator it = UpgradeTables.find(keyValue->second);
					if (it != UpgradeTables.end())
					{
						for (UpgradeTableInfoMap::const_iterator InfoIter = it->second.begin(), InfoEndIter = it->second.end(); InfoIter != InfoEndIter; ++InfoIter)
						{
							targetList.push_back(InfoIter->first);
						}
					}
				}

				const Database::Info::UpgradeTableInfo* UpgradeInfoManager::Retrieve(const Database::Info::ItemInfo* potionItem, const Database::Info::ItemInfo* targetItem)
				{
					if (potionItem == NULL)
						return NULL;

					if (targetItem == NULL)
						return NULL;

					ActionInfo::StateList::const_iterator i = std::find_if(potionItem->States.begin(), potionItem->States.end(), ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());
					if (i == potionItem->States.end())
						return NULL;

					std::map<String, String>::const_iterator keyValue = i->params.find(L"Table");
					if (keyValue == i->params.end())
						return NULL;

					UpgradeTableMap::const_iterator it = UpgradeTables.find(keyValue->second);
					if (it != UpgradeTables.end())
					{
						UpgradeTableInfoMap::const_iterator findIter = it->second.find(targetItem->Hash);
						if (findIter != it->second.end())
						{
							return &findIter->second;
						}
					}

					NewUpgradeTableMap::const_iterator newIt = NewUpgradeTables.find(keyValue->second);
					if (newIt != NewUpgradeTables.end())
					{
						UpgradeInfoList::const_iterator findIter = std::find_if(newIt->second.begin(), newIt->second.end(), UpgradeTableInfo::NewInfoFinder(targetItem->Limits.Class, targetItem->EquipParts));
						if (findIter != newIt->second.end())
						{
							return &(*findIter);
						}
					}
					return NULL;
				}
				
			}
		}
	}
}
