#ifndef CashShopBarter_H
#define CashShopBarter_H
#pragma once
#ifndef Serializer_H
#include <Core/Serializer/Serializer.h>
#endif // !Serializer_H
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct CashStampInfo
				{
					uint32	Hash;
					uint64	Fee;
					bool	AutoResponse;
					uint8	MaxAttachedItems;
					uint64	MaxAttachedMoney;
					int		ExpiredDay;// ByDay
					uint32	Delay;  // ByMinute

					struct Finder
					{
						uint32 Hash;
						Finder(uint32 h) :Hash(h) {}
						bool operator()(const CashStampInfo& info) const;
					};

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
				struct BarterItemInfo
				{
					uint32	BuyItemId;
					uint32	BuyItemPrice;
					uint32	NeedItemId;
					uint16	NeedItemCount;

					struct Finder
					{
						uint32 barterItemId;
						inline Finder(uint32 h) :barterItemId(h) {}
						bool operator()(const BarterItemInfo& info) const;
					};

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				//ShopInfo

				struct ShopInfo
				{
				public:
					struct CategoryTabInfo
					{
						std::wstring			DisplayName;
						std::wstring			ToolTip;
						uint32					ShopCategoryWindowInfoHash;
						std::wstring			CashSlotID;
						uint8					ShopItemWindowType;

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					struct Category : public CategoryTabInfo
					{
						struct Item {
							uint32	ItemHash;
							uint16	BuyCount;
						public:
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);

							struct Finder
							{
								uint32 What;
								inline Finder(uint32 what)
									: What(what)
								{
								}

								bool operator()(ShopInfo::Category::Item& lhs);
							};

							std::vector< std::wstring > Tags;
						};
						typedef std::vector<Item> ItemList;
						ItemList	Items;
					public:
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<Category> CategoryList;
					CategoryList	Categorys;
					std::wstring	DisplayName;
					std::wstring	AmbienceSound;
					uint32 BackWindowInfoHash;
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
#endif // ! CashShopBarter_H