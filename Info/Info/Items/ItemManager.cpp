#include "ItemManager.h"
#include <Core/Resource/Xml/XmlParser.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				ItemInfoManager::ItemInfoManager()
				{
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
					out.Begin(L"AllM::XRated::Database::Info::ItemInfoManager");
					out.Write(L"Items", Items);
					out.Write(L"UnidentifiedItems", UnidentifiedItems);
					out.Write(L"CategoryList", CategoryList);
					out.Write(L"SetItemList", SetItemList);
				}

				void ItemInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemInfoManager");
					in.Read(L"Items", Items);
					in.Read(L"UnidentifiedItems", UnidentifiedItems);
					in.Read(L"CategoryList", CategoryList);
					in.Read(L"SetItemList", SetItemList);
				}
			}
		}
	}
}