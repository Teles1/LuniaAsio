#pragma once
#include <Info/Info/Items/ItemManager.h>
#include <Info/Info/IndexedInfoManager.h>
#include <Core/FileIO/FileStream.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class IndexedItemInfoManager :
					public
					Lunia::XRated::Database::Info::ItemInfoManager
					//Lunia::XRated::Database::Info::CompressedItemInfoManager
				{

				public:
					void LoadBinaryData();

					ItemInfo* Retrieve(uint32 hash);
					ItemInfo* Retrieve(const wchar_t* id);
					UnidentifiedItemInfo* RetrieveUnidentifiedItem(uint32 hash);
					UnidentifiedItemInfo* RetrieveUnidentifiedItem(const wchar_t* id);

					virtual bool Remove(const wchar_t* id);
					virtual bool Remove(uint32 hash);

				public:
					void MakeIndexAndSave();
				public:
					void ClearCache() { Items.clear(); UnidentifiedItems.clear(); }
				protected:
					com_ptr<Loader>			loader;
					com_ptr<IndexedManagerWithMap>	itemManager;
					com_ptr<IndexedManagerWithMap>	unidentifiedItemManager;
					com_ptr<IndexedManagerWithElement> categoryManager;
				};
			}
		}
	}
}