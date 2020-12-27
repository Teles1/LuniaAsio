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

					void LoadCompressedBinary();
					void SaveCompressedB();
					void SaveCompressedXml();
					void SaveItemsCbf();
					ItemInfo* Retrieve(uint32 hash);
					ItemInfo* Retrieve(const wchar_t* id);
					UnidentifiedItemInfo* RetrieveUnidentifiedItem(uint32 hash);
					UnidentifiedItemInfo* RetrieveUnidentifiedItem(const wchar_t* id);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);
					void Init(bool bForceXmlData);
				};
			}
		}
	}
}