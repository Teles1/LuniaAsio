#include "DailyItemInfoManager.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void DailyItem::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::DailyItemInfo");
					out.Write(L"ItemHash", ItemHash);
					out.Write(L"SlotCount", SlotCount);
				}
				void DailyItem::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::DailyItemInfo");
					in.Read(L"ItemHash", ItemHash);
					in.Read(L"SlotCount", SlotCount);
				}
				const std::vector<DailyItem>& DailyItemInfoManager::GetDailyItem(const uint32& sourceItem, const DateTime::Week::type& week) const
				{
					DailyItemsFromItemTable::const_iterator dailyItemsIter = dailyItemsFromItemTable.find(sourceItem);
					if (dailyItemsIter != dailyItemsFromItemTable.end()) {
						return dailyItemsIter->second.at(week);
					}
					throw Exception(L"can not found daily item info : {0}", sourceItem);
				}
				const DailyItem* DailyItemInfoManager::Get(const uint32& sourceItem, const DateTime::Week::type& week, const uint32& hash) const
				{
					auto dailyItemsIter = dailyItemsFromItemTable.find(sourceItem);
					if (dailyItemsIter != dailyItemsFromItemTable.end()) {
						for(auto& iter : dailyItemsIter->second.at(week)) 
							if (iter.ItemHash == hash)
								return &iter;
					}
					return nullptr;
				}
				void DailyItemInfoManager::Init(bool xml)
				{
					if (xml == true) {
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Enchant/DailyItems.xml");
						reader->Read(L"DailyItemsFromItemTable", dailyItemsFromItemTable);

					}
					else {
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/DailyItems.b");
						reader->Read(L"DailyItemsFromItemTable", dailyItemsFromItemTable);
					}
				}
				void DailyItemInfoManager::Save(bool xml)
				{
					if (xml == true) {
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Items/Enchant/DailyItems.xml");
						writer->Write(L"DailyItemsFromItemTable", dailyItemsFromItemTable);
					}
					else {
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/DailyItems.b");
						writer->Write(L"DailyItemsFromItemTable", dailyItemsFromItemTable);
					}
				}
			}
		}
	}
}