#pragma once
#include <Core/Serializer/Serializer.h>
#include <Core/Utils/DateTime.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct DailyItem
				{
					uint32	ItemHash;
					uint16	SlotCount;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
				class DailyItemInfoManager
				{
					typedef std::vector< std::vector<DailyItem> >	DailyItemsFromItem;
					typedef std::map<uint32, DailyItemsFromItem>	DailyItemsFromItemTable;


					DailyItemsFromItemTable				dailyItemsFromItemTable;
				public:
					const std::vector<DailyItem>& GetDailyItem(const uint32& sourceItem, const DateTime::Week::type& week) const;
					const DailyItem* Get(const uint32& sourceItem, const DateTime::Week::type& week, const uint32& hash) const;
				public:
					void Init(bool xml);
					void Save(bool xml);
				};
			}
		}
	}
}