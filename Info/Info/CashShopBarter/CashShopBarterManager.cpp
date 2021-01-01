#include "CashShopBarterManager.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				const CashStampInfo* CashStampInfoManager::Get(uint32 hash)
				{
					CashStampIter it = std::find_if(infos.begin(), infos.end(), CashStampInfo::Finder(hash));
					if (it != infos.end())
					{
						return &*it;
					}
					else
					{
						return NULL;
					}
				}
				uint32 CashStampInfoManager::GetCashShopStampHash()
				{
					return cashShopStampHash;
				}
				void CashStampInfoManager::Init(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Mail/CashStamps.xml");
						reader->Read(L"CashStamps", infos);
						reader->Read(L"CashShopStampHash", cashShopStampHash);
					}
					else
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/CashStamps.b");
						reader->Read(L"CashStamps", infos);
						reader->Read(L"CashShopStampHash", cashShopStampHash);
					}
				}
				void CashStampInfoManager::Save(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Items/Mail/CashStamps.xml");
						writer->Write(L"CashStamps", infos);
						writer->Write(L"CashShopStampHash", cashShopStampHash);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/CashStamps.b");
						writer->Write(L"CashStamps", infos);
						writer->Write(L"CashShopStampHash", cashShopStampHash);
					}
				}

				void BarterItemInfoManager::Init(bool bForceXmlData)
				{
					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else {
						LoadBinaryData();
					}
				}

				BarterItemInfo* BarterItemInfoManager::Retrieve(uint32 itemid)
				{
					//return &BarterItems[itemid];
					return &BarterItems[itemid];
				}

				void BarterItemInfoManager::LoadXmlData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Shop/BarterShop.xml");
					reader->Read(L"barterItemInfos", BarterItems);
				}

				void BarterItemInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerBinaryStreamReader(L"Database/BarterItemInfos.b");
					reader->Read(L"barterItemInfos", BarterItems);
				}

				void BarterItemInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"BarterItemInfoManager");
					out.Write(L"barterItemInfos", BarterItems);
				}

				void BarterItemInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"BarterItemInfoManager");
					in.Read(L"barterItemInfos", BarterItems);
				}

				//ShopInfoTable
				ShopInfoTable::ShopInfoTable()
				{
				}

				ShopInfoTable::~ShopInfoTable()
				{
				}
				const ShopInfo* ShopInfoTable::Get(uint32 index)
				{
					Table::const_iterator iter = table.find(index);
					if (iter != table.end()) {
						return &(iter->second);
					}
					return NULL;
				}
				void ShopInfoTable::Init(bool xml)
				{
					if (xml == true) {
						std::map<uint32, std::wstring>	shopFileTable;
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Shop/ShopList.xml");
							reader->Read(L"ShopFileTable", shopFileTable);
						}

						std::map<uint32, std::wstring>::const_iterator iter = shopFileTable.begin();
						std::map<uint32, std::wstring>::const_iterator end = shopFileTable.end();
						while (iter != end) {
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(iter->second.c_str());
							reader->Read(L"ShopInfo", table[iter->first]);
							++iter;
						}
					}
					else {
						LoadBinaryData();
					}
				}
				void ShopInfoTable::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/Shop.b");
					reader->Read(L"ShopInfoTable", table);
				}
				void ShopInfoTable::Save(bool xml)
				{
					if (xml == true) {
						std::map<uint32, std::wstring>	shopFileTable;
						{
							if (Resource::ResourceSystemInstance().Exists(L"Database/Shop/ShopList.xml") == true) {
								Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Shop/ShopList.xml");
								reader->Read(L"ShopFileTable", shopFileTable);
							}
						}

						Table::const_iterator iter = table.begin();
						Table::const_iterator end = table.end();

						while (iter != end) {
							std::wstring file;
							std::map<uint32, std::wstring>::const_iterator fileIter = shopFileTable.find(iter->first);
							if (fileIter == shopFileTable.end()) {
								file = L"Shop_" + StringUtil::To<std::wstring>(iter->first) + L".xml";
							}
							else {
								file = fileIter->second;
							}

							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(file.c_str());
							writer->Write(L"ShopInfo", iter->second);
							++iter;
						}
					}
					else {
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/Shop.b");
						writer->Write(L"ShopInfoTable", table);
					}
				}

				void ShopInfoTable::Remove(uint32 hash)
				{
					if (!table.empty())
					{
						for (Table::iterator mIter = table.begin();
							mIter != table.end();
							++mIter)
						{
							for (ShopInfo::CategoryList::iterator vIter = mIter->second.Categorys.begin();
								vIter != mIter->second.Categorys.end();
								++vIter)
							{
								ShopInfo::Category::ItemList::iterator fIter =
									std::find_if(vIter->Items.begin(), vIter->Items.end(), ShopInfo::Category::Item::Finder(hash));

								if (fIter != vIter->Items.end())
								{
									vIter->Items.erase(fIter++);
								}
							}
						}
					}
				}

				std::map<uint32, ShopInfo>& ShopInfoTable::GetTable()
				{
					return table;
				}


				void ShopInfoTable::SortSellItemList()
				{
					shopToSellItems.clear();
					shopToSellItems.resize(table.size());

					ShopToSellItemList::iterator shopToSellItemIter = shopToSellItems.begin();

					Table::const_iterator iter = table.begin();
					Table::const_iterator end = table.end();
					while (iter != end) {

						SellItemLists& items = *shopToSellItemIter;

						ShopInfo::CategoryList::const_iterator categoryIter = iter->second.Categorys.begin();
						ShopInfo::CategoryList::const_iterator categoryEnd = iter->second.Categorys.end();
						while (categoryIter != categoryEnd) {
							ShopInfo::Category::ItemList::const_iterator itemIter = (*categoryIter).Items.begin();
							ShopInfo::Category::ItemList::const_iterator itemEnd = (*categoryIter).Items.end();
							while (itemIter != itemEnd) {
								items.push_back((*itemIter).ItemHash);
								++itemIter;
							}
							++categoryIter;
						}
						++iter;
						++shopToSellItemIter;
					}
				}
				bool ShopInfoTable::IsShopItem(uint32 shopType, uint32 hash) const
				{
					if (shopToSellItems.size() > shopType) {
						const SellItemLists& sellItems = (shopToSellItems.at(shopType));
						SellItemLists::const_iterator iter = std::find(sellItems.begin(), sellItems.end(), hash);
						if (iter != sellItems.end()) {
							return true;
						}
					}
					return false;
				}
			}
		}
	}
}