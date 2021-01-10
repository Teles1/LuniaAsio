#ifndef CashShopBarterManager_H
#define CashShopBarterManager_H
#pragma once
#ifndef CashShopBarter_H
#include <Info/Info/CashShopBarter/CashShopBarter.h>
#endif // !CashShopBarter_H

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class CashStampInfoManager
				{
					uint32 cashShopStampHash;
					std::vector< CashStampInfo > infos;
					typedef std::vector< CashStampInfo >::iterator CashStampIter;
				public:
					const CashStampInfo* Get(uint32 hash);
					uint32 GetCashShopStampHash();
					void Init(bool xml);
					void Save(bool xml);
				};
				class ShopInfoTable
				{
					typedef std::map<uint32, ShopInfo>	Table;
					Table	table;


					typedef std::vector<uint32>				SellItemLists;
					typedef std::vector< SellItemLists >	ShopToSellItemList;
					ShopToSellItemList						shopToSellItems;
				public:
					const ShopInfo* Get(uint32 index);
					void Init(bool xml);
					void Save(bool xml);

					void Remove(uint32 hash);
					std::map<uint32, ShopInfo>& GetTable();


				protected:
					virtual void LoadBinaryData();
				public:
					/** SortSellItemList.
						@bref : shop item list  sort to level to sell item list(origin date is sort to category/with visual).
					*/
					void SortSellItemList();
					/** IsShopItem.
						@bref : guild level to item validater
					*/
					bool IsShopItem(uint32 shopType, uint32 hash) const;
				public:
					ShopInfoTable();
					virtual ~ShopInfoTable();
				};

				class BarterItemInfoManager
				{
					std::map<uint32, BarterItemInfo> BarterItems;
				public:
					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();

					BarterItemInfo* Retrieve(uint32 itemid);
					//BarterItemInfo* Retrieve(uint32 itemid);
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}

#endif // !CashShopBarterManager_H