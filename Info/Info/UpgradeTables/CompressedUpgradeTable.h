#pragma once
#include "../CompressedLoader.h"
#include "UpgradeInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class CompressedUpgradeTableManager : public UpgradeInfoManager
				{
				public:
					typedef std::map<std::wstring, uint32, Less<std::wstring> > CompressedUpgradeTableMap;
					PositionData<CompressedUpgradeTableMap> compressedUpgradeTables;
					PositionData<CompressedUpgradeTableMap> compressedNewUpgradeTables;

					Resource::StreamReader compressedUpgradeCbf;
					std::vector<uint8> IndexedUpgradeTablesCompressed;
					std::vector<uint8> IndexedNewUpgradeTablesCompressed;

					void Init(bool forceXml = false);
					void LoadBinaryData();
					void LoadData();
					bool GetUpgradeTable(const uint32 templateOffset);
					bool GetNewUpgradeTable(const uint32 templateOffset);
					UpgradeTableInfo* Retrieve(const Database::Info::ItemInfo* potionItem, const Database::Info::ItemInfo* targetItem);

					const UpgradeTableInfo* RetrieveNewUpgrade(const Database::Info::ItemInfo* potionItem, const Database::Info::ItemInfo* targetItem);
				};
			}
		}
	}
}
