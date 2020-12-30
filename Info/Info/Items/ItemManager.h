#pragma once
#include<Info/Info/Items/Item.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class ItemInfoManager : public Serializer::ISerializable
				{
				protected:
					typedef std::map<uint32, ItemInfo, Less<uint32> > ItemInfoMap;
					typedef std::map<uint32, ItemInfo, Less<uint32> >::iterator ItemInfoMapIt;
					typedef std::map<uint32, UnidentifiedItemInfo, Less<uint32> > UnidentifiedItemInfoMap;

					ItemInfoMap Items;
					UnidentifiedItemInfoMap UnidentifiedItems;
					std::vector<ItemCategory> CategoryList;
					std::vector<SetItemInfo> SetItemList;
					std::map<uint8, std::map<uint16, uint32> >ComposeItemList;
					typedef std::map<uint8, std::map<uint16, uint32> >::iterator ComposeItemListIt;
					std::map<uint8, std::map<uint8, std::pair<uint16, uint16> > >ComposeGradeInfos;
					typedef std::map<uint8, std::map<uint8, std::pair<uint16, uint16> > >::iterator ComposeGradeInfosIt;

					std::map<uint32, UnidentifiedItemInfo> eventUnidentifiedItems;

				public:
				
					typedef std::map<uint32, ItemInfo, Less<uint32> >::iterator	ItemInfoIter;
					size_t Size() { return Items.size(); }
					ItemInfoIter Begin() { return Items.begin(); }
					ItemInfoIter End() { return Items.end(); }
				public:
					ItemInfoManager();
				
					struct Finder
					{
						std::wstring What;
						Finder(std::wstring what)
							: What(what)
						{
						}

						bool operator()(Lunia::XRated::UnidentifiedItemInfo::ProbabilityTable& lhs)
						{
							return (lhs.Id == What);
						}
					};

				public:
					void GetItemList(std::vector<std::wstring>& l);
					void GetUnidentifiedItemList(std::vector<std::wstring>& l);
				
					std::pair<uint16, uint16> GetComposeGrade(std::pair<const ItemInfo*, const ItemInfo*> input);
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};

			}
		}
	}
}