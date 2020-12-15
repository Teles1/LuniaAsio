#include "ItemManager.h"
#include <Core/Resource/Xml/XmlParser.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				ItemInfoManager::ItemInfoManager()
				{
				}

				void ItemInfoManager::Init(bool bForceXmlData)
				{
					Items.clear();
					CategoryList.clear();
					UnidentifiedItems.clear();
					eventUnidentifiedItems.clear();

					if (bForceXmlData) {
						LoadXmlData();
					}
					else { 
						std::wstring fullLoading;
						//Lunia::Config::ConfigInstance().Get(L"database", L"fullLoading", fullLoading, L"1");
						if (fullLoading == L"1")
							ItemInfoManager::LoadBinaryData();
						else
							LoadBinaryData();
					}
				}

				void ItemInfoManager::Add(const ItemInfo& info)
				{
					Items[StringUtil::Hash(info.Id.c_str())] = info;
				}


				void ItemInfoManager::Remove(uint32 hash) 
				{
					ItemInfoMap::iterator iter = Items.find(hash);
					if (iter != Items.end())
					{
						for (UnidentifiedItemInfoMap::iterator uIter = UnidentifiedItems.begin();
							uIter != UnidentifiedItems.end();
							++uIter)
						{
							Lunia::uint32 level = 1;
							Lunia::XRated::UnidentifiedItemInfo::ProbabilityTableList* aList = uIter->second.GetProbabilityTableForModify(level);

							while (aList != NULL)
							{
								Lunia::XRated::UnidentifiedItemInfo::ProbabilityTableList::iterator fIter = aList->begin();

								do
								{
									fIter = std::find_if(aList->begin(), aList->end(), Finder(Retrieve(hash)->Id));
									if (fIter != aList->end())
									{
										aList->erase(fIter++);
									}

								} while (fIter != aList->end());

								aList = uIter->second.GetProbabilityTableForModify(++level);
							}
						}

						Items.erase(iter++);
					}
					else
					{
						Logger::GetInstance().Exception(L"ItemInfoManager::Remove() - item hash %d not found. illegal remove procedure.", hash);
						throw Lunia::Exception(L"ItemInfoManager::Remove() - item hash not found. illegal remove procedure.");
					}
				}

				void ItemInfoManager::AddUnidentifiedItemList(const std::map<uint32, UnidentifiedItemInfo>& unidentifiedItemList)
				{
					if (unidentifiedItemList.empty()) return;

					std::map<uint32, UnidentifiedItemInfo>::const_iterator iter = unidentifiedItemList.begin();
					while (iter != unidentifiedItemList.end())
					{
						Logger::GetInstance().Warn(L"UnidentifiedItemInfo Hash = %d", iter->first);
						UnidentifiedItems[iter->first] = iter->second;
						eventUnidentifiedItems[iter->first] = iter->second;

						++iter;
					}
				}

				void ItemInfoManager::ReloadEventUnidentifiedItemList()
				{
					if (eventUnidentifiedItems.empty()) return;

					std::map<uint32, UnidentifiedItemInfo>::const_iterator iter = eventUnidentifiedItems.begin();
					while (iter != eventUnidentifiedItems.end())
					{
						Logger::GetInstance().Warn(L"Reload UnidentifiedItemInfo Hash = %d", iter->first);
						UnidentifiedItems[iter->first] = iter->second;

						++iter;
					}
				}

				void ItemInfoManager::LoadXmlData()
				{
					
					{
						com_ptr<Lunia::Resource::IStream> stream = Lunia::Resource::ResourceSystemInstance().OpenStream(L"Database/Items/ItemList.xml");
						int len = stream->GetSize();
						wchar_t* buffer = (wchar_t*)new char[len + 2];
						ZeroMemory(buffer, len + 2);
						stream->Read((unsigned char*)buffer, len);

						Lunia::Xml::Parser parser;
						try {
							parser.Parse(buffer);
						}
						catch (Lunia::Xml::XmlException ex) {
							Logger::GetInstance().Info(L"[ItemInfoManager::Init] XML Parsing Error.[%s]", ex.GetErrorString());
							return;
						}

						Lunia::Xml::ElementCollection col;
						parser.FindElements(L"ItemList", &col);
						auto iter = col.begin();
						
						auto elem = *col.begin();

						for (iter = elem->SubElements.begin(); iter != elem->SubElements.end(); ++iter)
							LoadItemTemplates((*iter)->Attributes[L"path"].c_str());

						delete[]buffer;
					}

					
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Items/ItemCategory.xml");
					reader->Read(L"Categorys", CategoryList);


					
					{
						com_ptr<Lunia::Resource::IStream> stream = Lunia::Resource::ResourceSystemInstance().OpenStream(L"Database/Items/UnidentifiedItemList.xml");
						int len = stream->GetSize();
						wchar_t* buffer = (wchar_t*)new char[len + 2];
						ZeroMemory(buffer, len + 2);
						stream->Read((unsigned char*)buffer, len);

						Lunia::Xml::Parser parser;
						try {
							parser.Parse(buffer);
						}
						catch (Lunia::Xml::XmlException ex) {
							Logger::GetInstance().Warn(L"[ItemInfoManager::Init] XML Parsing Error.[%s]", ex.GetErrorString());
							return;
						}

						Lunia::Xml::ElementCollection col;
						parser.FindElements(L"UnidentifiedItemList", &col);
						auto iter = col.begin();
						auto elem = *col.begin();

						for (iter = elem->SubElements.begin(); iter != elem->SubElements.end(); ++iter)
							LoadUnidentfiedItemTemplates((*iter)->Attributes[L"path"].c_str());

						delete[]buffer;
					}
					
					
					

					
					
				}

				std::pair<uint16, uint16> ItemInfoManager::GetComposeGrade(std::pair<const ItemInfo*, const ItemInfo*> input) {
					std::pair<uint16, uint16> output(0, 0);
					if (input.first->ComposeData.Type == 0 || input.second->ComposeData.Type == 0) {
						Logger::GetInstance().Warn(L"ItemInfoManager::GetComposeGrade() - Compose Type = 0, %d, %d", input.first->Hash, input.second->Hash);
						return output;
					}

					if (input.first->ComposeData.Type != input.second->ComposeData.Type) {
						Logger::GetInstance().Warn(L"ItemInfoManager::GetComposeGrade() - Type does not match %d, %d", input.first->Hash, input.second->Hash);
						return output;
					}
					/* Look hor ItemHash */
					for (ComposeItemListIt it = ComposeItemList.begin(); it != ComposeItemList.end(); it++)
						if (it->first == input.first->ComposeData.Type) {
							std::map<Lunia::uint16, Lunia::uint32>::iterator itt;
							for (itt = ComposeItemList[input.first->ComposeData.Type].begin();
								itt != ComposeItemList[input.first->ComposeData.Type].end(); itt++) {
								if (itt->second == input.first->Hash)
									output.first = itt->first;
								else if (itt->second == input.second->Hash)
									output.second = itt->first;
								if (output.first != uint16(0) && output.second != uint16(0))
									return output;
							}
							Logger::GetInstance().Warn(L"ItemInfoManager::GetComposeGrade() - Could not find compose data for input itens %d, %d", input.first->Hash, input.second->Hash);
							return output;
						}

					Logger::GetInstance().Warn(L"ItemInfoManager::GetComposeGrade() - Could not find compose Type for input itens %d, %d", input.first->Hash, input.second->Hash);
					return output;
				}

				void ItemInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/ItemInfos.b");
					reader->Read(L"ItemInfoManager", *this);
				}

				void ItemInfoManager::LoadUnidentifiedData()
				{
					Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"UnidentifiedItems.b");
					reader->Read(L"UnidentifiedItems", UnidentifiedItems);
				}
				void ItemInfoManager::SaveUnidentifiedData(bool bForceXml)
				{
					if (bForceXml) {
						Resource::SerializerStreamWriter writer = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"UnidentifiedItems.xml");
						writer->Write(L"UnidentifiedItems", UnidentifiedItems);
					}
					else {
						Resource::SerializerStreamWriter writer = Lunia::Resource::ResourceSystemInstance().CreateSerializerBinaryStreamWriter(L"UnidentifiedItems_gen.b");
						writer->Write(L"UnidentifiedItems", UnidentifiedItems);
					}
				}

				void ItemInfoManager::LoadItemTemplates(const wchar_t* xml)
				{
					ItemInfoMap temp;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"Items", temp);

					Items.insert(temp.begin(), temp.end());
				}

				void ItemInfoManager::LoadUnidentfiedItemTemplates(const wchar_t* xml)
				{
					UnidentifiedItemInfoMap temp;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"UnidentifiedItems", temp);

					
					UnidentifiedItemInfoMap::iterator iter = temp.begin();
					UnidentifiedItemInfoMap::iterator end = temp.end();
					while (iter != end) {
						iter->second.Sort();
						++iter;
					}

					UnidentifiedItems.insert(temp.begin(), temp.end());
				}

				ItemInfo* ItemInfoManager::Retrieve(uint32 hash)
				{
					ItemInfoMap::iterator i = Items.find(hash);
					if (i != Items.end())
						return &(*i).second;
					return 0;
				}

				ItemInfo* ItemInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(Lunia::StringUtil::Hash(id));
				}

				UnidentifiedItemInfo* ItemInfoManager::RetrieveUnidentifiedItem(uint32 hash)
				{
					UnidentifiedItemInfoMap::iterator i = UnidentifiedItems.find(hash);
					if (i == UnidentifiedItems.end())
						return NULL;
					return &(*i).second;
				}

				UnidentifiedItemInfo* ItemInfoManager::RetrieveUnidentifiedItem(const wchar_t* id)
				{
					return RetrieveUnidentifiedItem(Lunia::StringUtil::Hash(id));
				}

				void ItemInfoManager::GetItemList(std::vector<std::wstring>& l)
				{
					ItemInfoMap::iterator i = Items.begin(), end = Items.end();
					while (i != end) {
						l.push_back((*i).second.Id);
						++i;
					}
				}

				void ItemInfoManager::GetUnidentifiedItemList(std::vector<std::wstring>& l)
				{
					UnidentifiedItemInfoMap::iterator i = UnidentifiedItems.begin(), end = UnidentifiedItems.end();
					while (i != end) {
						l.push_back((*i).second.Id);
						++i;
					}
				}

				void ItemInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Lunia::XRated::Database::Info::ItemInfoManager");
					out.Write(L"Items", Items);
					out.Write(L"UnidentifiedItems", UnidentifiedItems);
					out.Write(L"CategoryList", CategoryList);
					out.Write(L"SetItemList", SetItemList);
				}

				void ItemInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Lunia::XRated::Database::Info::ItemInfoManager");
					in.Read(L"Items", Items);
					in.Read(L"UnidentifiedItems", UnidentifiedItems);
					in.Read(L"CategoryList", CategoryList);
					in.Read(L"SetItemList", SetItemList);
				}

			}
		}
	}
}