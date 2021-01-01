#include "CashShopBarter.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void CashStampInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::CashStampInfo");
					out.Write(L"ItemHash", Hash);
					out.Write(L"Fee", Fee);
					out.Write(L"AutoResponse", AutoResponse);
					out.Write(L"MaxAttachedItems", MaxAttachedItems);
					out.Write(L"MaxAttachedMoney", MaxAttachedMoney);
					out.Write(L"ExpiredDay", ExpiredDay);
					out.Write(L"Delay", Delay);
				}
				void CashStampInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::CashStampInfo");
					in.Read(L"ItemHash", Hash);
					in.Read(L"Fee", Fee);//,				0);
					in.Read(L"AutoResponse", AutoResponse);//,		false);
					in.Read(L"MaxAttachedItems", MaxAttachedItems);//,	1);
					in.Read(L"MaxAttachedMoney", MaxAttachedMoney);//,	100000);
					in.Read(L"ExpiredDay", ExpiredDay);//,			14);
					in.Read(L"Delay", Delay);//,				60);
				}
				/* 3.1 by ultimate */
				void BarterItemInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::BarterItemInfo");
					out.Write(L"BuyItemId", BuyItemId);
					out.Write(L"BuyItemPrice", BuyItemPrice);
					out.Write(L"NeedItemId", NeedItemId);
					out.Write(L"NeedItemCount", NeedItemCount);
				}

				void BarterItemInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::BarterItemInfo");
					in.Read(L"BuyItemId", BuyItemId);
					in.Read(L"BuyItemPrice", BuyItemPrice);
					in.Read(L"NeedItemId", NeedItemId);
					in.Read(L"NeedItemCount", NeedItemCount);
				}

				void ShopInfo::Category::Item::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ShopInfo::Category::Item");
					out.Write(L"ItemHash", ItemHash);
					out.Write(L"BuyCount", BuyCount);
					out.Write(L"Tags", Tags);
				}
				void ShopInfo::Category::Item::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ShopInfo::Category::Item");
					in.Read(L"ItemHash", ItemHash);
					in.Read(L"BuyCount", BuyCount, static_cast<uint16>(1));
					in.Read(L"Tags", Tags, std::vector< std::wstring >());
				}
				void ShopInfo::CategoryTabInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ShopInfo::CategoryTabInfo");
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"ToolTip", ToolTip);
					out.Write(L"ShopCategoryWindowInfoHash", ShopCategoryWindowInfoHash);
					out.Write(L"CashSlotID", CashSlotID);
					out.Write(L"ShopItemWindowType", ShopItemWindowType);
				}
				void ShopInfo::CategoryTabInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ShopInfo::CategoryTabInfo");
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"ToolTip", ToolTip);
					in.Read(L"ShopCategoryWindowInfoHash", ShopCategoryWindowInfoHash);
					in.Read(L"CashSlotID", CashSlotID, std::wstring());
					in.Read(L"ShopItemWindowType", ShopItemWindowType, (uint8)0);
				}

				void ShopInfo::Category::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ShopInfo::Category");
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"ToolTip", ToolTip);
					out.Write(L"ShopCategoryWindowInfoHash", ShopCategoryWindowInfoHash);
					out.Write(L"CashSlotID", CashSlotID);
					out.Write(L"ShopItemWindowType", ShopItemWindowType);
					out.Write(L"Items", Items);
				}
				void ShopInfo::Category::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ShopInfo::Category");
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"ToolTip", ToolTip);
					in.Read(L"ShopCategoryWindowInfoHash", ShopCategoryWindowInfoHash);
					in.Read(L"CashSlotID", CashSlotID, std::wstring());
					in.Read(L"ShopItemWindowType", ShopItemWindowType, (uint8)0);
					in.Read(L"Items", Items);
				}

				void ShopInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ShopInfo");
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"Categorys", Categorys);
					out.Write(L"AmbienceSound", AmbienceSound);
					out.Write(L"BackWindowInfoHash", BackWindowInfoHash);
				}
				void ShopInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ShopInfo");
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"Categorys", Categorys);
					in.Read(L"AmbienceSound", AmbienceSound);
					in.Read(L"BackWindowInfoHash", BackWindowInfoHash);
				}
				bool ShopInfo::Category::Item::Finder::operator()(ShopInfo::Category::Item& lhs) {
					return (lhs.ItemHash == What);
				}
				bool BarterItemInfo::Finder::operator()(const BarterItemInfo& info) const { 
					return info.BuyItemId == barterItemId; 
				}
				bool CashStampInfo::Finder::operator()(const CashStampInfo& info) const { 
					return info.Hash == Hash; 
				}
}
		}
	}
}