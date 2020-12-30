#pragma once
#include <Info/Info/Items/CompressedItemInfoManager.h>
#include "UpgradeTable.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class UpgradeInfoManager
				{
				protected:
					typedef std::map<uint32 /*targetItemHash*/, Info::UpgradeTableInfo > UpgradeTableInfoMap;
					typedef std::map< std::wstring /*UpgradeTableName*/, UpgradeTableInfoMap > UpgradeTableMap;
					UpgradeTableMap UpgradeTables;

					typedef std::list< UpgradeTableInfo > UpgradeInfoList;
					typedef std::map< std::wstring /*UpgradeTableName*/, UpgradeInfoList > NewUpgradeTableMap;
					NewUpgradeTableMap	NewUpgradeTables;

				public:
					bool IsPotionItem(const Database::Info::ItemInfo* itemInfo);
					bool IsNewPotionItem(const Database::Info::ItemInfo* itemInfo);

					bool IsTargetItem(const Database::Info::ItemInfo* potion, const Database::Info::ItemInfo* target);
					void GetTargetList(const Database::Info::ItemInfo* potionItem, std::list<uint32/*target Hash*/>& targetList);
					const Database::Info::UpgradeTableInfo* Retrieve(const Database::Info::ItemInfo* potionItem, const Database::Info::ItemInfo* targetItem);
				};
			}
		}
	}
}
