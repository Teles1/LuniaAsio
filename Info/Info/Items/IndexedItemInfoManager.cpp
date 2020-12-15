#include "IndexedItemInfoManager.h"
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void IndexedItemInfoManager::LoadBinaryData()
				{
					loader = CreateLoader(L"Database/ItemInfos.b");

					itemManager = CreateIndexedManagerWithMap(loader);

					categoryManager = CreateIndexedManagerWithElement(loader, L"Categorys");

					unidentifiedItemManager = CreateIndexedManagerWithMap(loader);

					itemManager->Load(L"Database/ItemInfosIndex.b");

					categoryManager->Load(L"Database/ItemCategoryListIndex.b");

					categoryManager->Get(CategoryList);

					unidentifiedItemManager->Load(L"Database/UnIdentifiedItemInfosIndex.b");
				}

				void IndexedItemInfoManager::MakeIndexAndSave()
				{
					Lunia::XRated::Database::Info::ItemInfo data1;
					Lunia::XRated::UnidentifiedItemInfo data2;

					loader = CreateLoader(L"Database/ItemInfos.b");

					itemManager = CreateIndexedManagerWithMap(loader, data1);

					unidentifiedItemManager = CreateIndexedManagerWithMap(loader, data2);

					categoryManager = CreateIndexedManagerWithElement(loader, L"Categorys", CategoryList);

					itemManager->Save(L"Database/ItemInfosIndex.b");

					categoryManager->Save(L"Database/ItemCategoryListIndex.b");

					unidentifiedItemManager->Save(L"Database/UnIdentifiedItemInfosIndex.b");
				}

				Lunia::XRated::Database::Info::ItemInfo* IndexedItemInfoManager::Retrieve(uint32 hash)
				{
					
					ItemInfoMap::iterator ita = Items.find(hash);
					if (ita != Items.end())
						return &ita->second;
					else
					{
						if (itemManager == NULL) return NULL;

						Lunia::XRated::Database::Info::ItemInfo data;
						
						if (itemManager->Get(hash, data))
						{
							
							Items[hash] = data;
							return &Items[hash];
						}
						else
							return NULL; 
					}
				}

				
				Lunia::XRated::Database::Info::ItemInfo* IndexedItemInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(Lunia::StringUtil::Hash(id));
				}

				Lunia::XRated::UnidentifiedItemInfo* IndexedItemInfoManager::RetrieveUnidentifiedItem(Lunia::uint32 hash)
				{
					
					UnidentifiedItemInfoMap::iterator ita = UnidentifiedItems.find(hash);
					if (ita != UnidentifiedItems.end())
						return &ita->second;
					else
					{
						if (unidentifiedItemManager == NULL) return NULL;

						Lunia::XRated::UnidentifiedItemInfo data;
						
						if (unidentifiedItemManager->Get(hash, data))
						{
							
							UnidentifiedItems[hash] = data;
							return &UnidentifiedItems[hash];
						}
						else
						{
							
							std::map<uint32, UnidentifiedItemInfo>::iterator iter = eventUnidentifiedItems.find(hash);
							if (iter != eventUnidentifiedItems.end())
								return &iter->second;
							else
								return NULL; 
						}
					}
				}

				
				Lunia::XRated::UnidentifiedItemInfo* IndexedItemInfoManager::RetrieveUnidentifiedItem(const wchar_t* id)
				{
					return RetrieveUnidentifiedItem(Lunia::StringUtil::Hash(id));
				}

				
				bool IndexedItemInfoManager::Remove(Lunia::uint32 hash)
				{
					ItemInfoMap::iterator it = Items.find(hash);
					if (it != Items.end())
					{
						Items.erase(it);
						return true;
					}

					UnidentifiedItemInfoMap::iterator ita = UnidentifiedItems.find(hash);
					if (ita != UnidentifiedItems.end())
					{
						UnidentifiedItems.erase(ita);
						return true;
					}

					return false;
				}

				
				bool IndexedItemInfoManager::Remove(const wchar_t* id)
				{
					return Remove(Lunia::StringUtil::Hash(id));
				}

			}
		}
	}
}