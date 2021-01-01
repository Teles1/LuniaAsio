#pragma once
#include <Info/Info/Items/ItemManager.h>
#include <Info/Info/CompressedLoader.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class CompressedItemInfoManager : public Lunia::XRated::Database::Info::ItemInfoManager, CompressedLoader
				{
				public:
					typedef PositionData<std::map<uint32, uint32>> PositionDat;
					typedef UnidentifiedItemInfoMap::iterator UnidentifiedItemInfoMapIt;

					PositionDat UnidentifiedItemInfoCompressed;
					PositionDat CompressedItems;

					Resource::StreamReader compressedItemsCBF;
					std::vector<uint8> IndexedItemsCompressed;
					std::vector<uint8> IndexedUnidentifiedCompressed;

					void Init(bool forceXml = false);
					void LoadData();
					void LoadBinaryData();
					ItemInfo* Retrieve(const uint32& hash);
					ItemInfo* Retrieve(const wchar_t* name);
					UnidentifiedItemInfo* RetrieveUnidentifiedItem(const uint32& hash);
					UnidentifiedItemInfo* RetrieveUnidentifiedItem(const wchar_t* id);

				private:
					bool GetUnidentifiedItems(const uint32& templateOffset);
					bool GetItems(const uint32& templateOffset);
				};
			}
		}
	}
}