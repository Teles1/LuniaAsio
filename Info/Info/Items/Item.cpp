#include "Item.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void ItemCategory::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ItemCategory");
					out.Write(L"categoryType", (int)categoryType);
					out.Write(L"cooldown", cooldown);
				}

				void ItemCategory::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemCategory");
					in.Read(L"categoryType", (int&)categoryType);
					in.Read(L"cooldown", cooldown);
				}

				void ItemInfo::Limit::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ItemInfo::Limit");
					out.WriteEnum(L"Class", Class);
					out.Write(L"MajorStat", MajorStat);
					out.Write(L"Level", Level);
					out.Write(L"MaxLevel", MaxLevel);
					out.Write(L"PvpLevel", PvpLevel);
					out.Write(L"MaxPvpLevel", MaxPvpLevel);
					out.Write(L"WarLevel", WarLevel);
					out.Write(L"MaxWarLevel", MaxWarLevel);
					out.Write(L"StoredLevel", StoredLevel);
					out.Write(L"MaxStoredLevel", MaxStoredLevel);
					out.Write(L"RebirthCount", RebirthCount);
					out.Write(L"MaxRebirthCount", MaxRebirthCount);
					out.Write(L"Str", Str);
					out.Write(L"Int", Int);
					out.Write(L"Dex", Dex);
					out.Write(L"Vit", Vit);
					out.Write(L"StageIds", StageIds);
					out.Write(L"UnableStageIds", UnableStageIds);
					out.Write(L"StageType", StageType);
					out.Write(L"BFatal", BFatal);
				}

				void ItemInfo::Limit::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemInfo::Limit");
					in.ReadEnum(L"Class", Class);
					in.Read(L"MajorStat", MajorStat, 0);
					in.Read(L"Level", Level, 1);
					if (Level < 1) Level = 1;
					in.Read(L"MaxLevel", MaxLevel, (int)Constants::MaxLevel);
					in.Read(L"PvpLevel", PvpLevel, 1);
					if (PvpLevel < 1) PvpLevel = 1;
					in.Read(L"MaxPvpLevel", MaxPvpLevel, (int)Constants::MaxLevel);
					in.Read(L"WarLevel", WarLevel, 1);
					if (WarLevel < 1) WarLevel = 1;
					in.Read(L"MaxWarLevel", MaxWarLevel, (int)Constants::MaxLevel);
					in.Read(L"StoredLevel", StoredLevel, 1);
					if (StoredLevel < 1)
						StoredLevel = 1;
					in.Read(L"MaxStoredLevel", MaxStoredLevel, (int)Constants::StoredMaxLevel);
					in.Read(L"RebirthCount", RebirthCount, 0);
					if (RebirthCount < 0)
						RebirthCount = 0;
					in.Read(L"MaxRebirthCount", MaxRebirthCount, (int)Constants::RebirthMaxCount);
					in.Read(L"Str", Str);
					in.Read(L"Int", Int);
					in.Read(L"Dex", Dex);
					in.Read(L"Vit", Vit);
					in.Read(L"StageIds", StageIds, std::vector< std::wstring >());
					in.Read(L"UnableStageIds", UnableStageIds, std::vector< std::wstring >());
					in.Read(L"StageType", StageType, (uint32)(Constants::GameTypes::CooperativeGameTypeMask + Constants::GameTypes::PvpGameTypeMask + Constants::GameTypes::PeacefulGameTypeMask));
					in.Read(L"BFatal", BFatal, true);
				}

				void ItemInfo::Attribute::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ItemInfo::Attribute");
					out.Write(L"TRADE", (Attr & ATTRIBUTES::TRADE) ? (uint16)1 : (uint16)0);
					out.Write(L"TRADEAFTERUSE", (Attr & ATTRIBUTES::TRADEAFTERUSE) ? (uint16)1 : (uint16)0);
					out.Write(L"CONSUME", (Attr & ATTRIBUTES::CONSUME) ? (uint16)1 : (uint16)0);
					out.Write(L"CASH", (Attr & ATTRIBUTES::CASH) ? (uint16)1 : (uint16)0);
					out.Write(L"SAVE", (Attr & ATTRIBUTES::SAVE) ? (uint16)1 : (uint16)0);
					out.Write(L"USE", (Attr & ATTRIBUTES::USE) ? (uint16)1 : (uint16)0);
					out.Write(L"ENCHANTPARTS", (Attr & ATTRIBUTES::ENCHANTPARTS) ? (uint16)1 : (uint16)0);
					out.Write(L"ACQUIREBYTURN", (Attr & ATTRIBUTES::ACQUIREBYTURN) ? (uint16)1 : (uint16)0);
					out.Write(L"STAGEITEM", (Attr & ATTRIBUTES::STAGEITEM) ? (uint16)1 : (uint16)0);
					out.Write(L"USEEX", (Attr & ATTRIBUTES::USEEX) ? (uint16)1 : (uint16)0);
					out.Write(L"PASSIVE", (Attr & ATTRIBUTES::PASSIVE) ? (uint16)1 : (uint16)0);
					out.Write(L"USEWHENGHOST", (Attr & ATTRIBUTES::USEWHENGHOST) ? (uint16)1 : (uint16)0);
					out.Write(L"EXISTWHENEXPIRED", (Attr & ATTRIBUTES::EXISTWHENEXPIRED) ? (uint16)1 : (uint16)0);
					out.Write(L"TRADEWHENEXPIRED", (Attr & ATTRIBUTES::TRADEWHENEXPIRED) ? (uint16)1 : (uint16)0);
					out.Write(L"GUILDITEM", (Attr & ATTRIBUTES::GUILDITEM) ? (uint16)1 : (uint16)0);
					out.Write(L"ACQUIREBYDICE", (Attr & ATTRIBUTES::ACQUIREBYDICE) ? (uint16)1 : (uint16)0);
					out.Write(L"ADDINQUICKSLOT", (Attr & ATTRIBUTES::ADDINQUICKSLOT) ? (uint16)1 : (uint16)0);

				}

				ItemInfo::RetTypeOfAttribute ConvertToAttrType(uint16 value)
				{
					return (value > 0) ? 1 : 0;
				}

				void ItemInfo::Attribute::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemInfo::Attribute");
					uint16 attr = 0;
					in.Read(L"TRADE", attr);
					Attr = ConvertToAttrType(attr);
					in.Read(L"TRADEAFTERUSE", attr);
					Attr += ConvertToAttrType(attr) << 1;
					in.Read(L"CONSUME", attr);
					Attr += ConvertToAttrType(attr) << 2;
					in.Read(L"CASH", attr);
					Attr += ConvertToAttrType(attr) << 3;
					in.Read(L"SAVE", attr);
					Attr += ConvertToAttrType(attr) << 4;
					in.Read(L"USE", attr);
					Attr += ConvertToAttrType(attr) << 5;
					in.Read(L"ENCHANTPARTS", attr);
					Attr += ConvertToAttrType(attr) << 6;
					in.Read(L"ACQUIREBYTURN", attr);
					Attr += ConvertToAttrType(attr) << 7;
					in.Read(L"STAGEITEM", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 8;
					in.Read(L"USEEX", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 9;
					in.Read(L"PASSIVE", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 10;
					in.Read(L"USEWHENGHOST", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 11;
					in.Read(L"EXISTWHENEXPIRED", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 12;
					in.Read(L"TRADEWHENEXPIRED", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 13;
					in.Read(L"GUILDITEM", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 14;
					in.Read(L"ACQUIREBYDICE", attr, (uint16)0);
					Attr += ConvertToAttrType(attr) << 15;
					in.Read(L"ADDINQUICKSLOT", attr, (uint16)0);

					Attr += ConvertToAttrType(attr) << 16;
				}

				void ItemInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::ItemInfo");
					out.Write(L"Id", Id);
					out.Write(L"ShopType", (int)ShopType);
					out.Write(L"Sellers", Sellers);
					out.Write(L"SellCategory", SellCategory);
					out.Write(L"Price", Price);
					out.Write(L"SellPrice", SellPrice);
					out.Write(L"SellCount", SellCount);
					out.Write(L"LadderPoint", LadderPoint);
					out.Write(L"MarketCategory", (int)MarketCategory);
					out.Write(L"ItemType", (int)ItemType);
					out.Write(L"MaxCnt", MaxCnt);
					out.Write(L"MaxUse", MaxUse);
					out.Write(L"Attributes", Attributes);
					out.Write(L"EquipParts", (int)EquipParts);
					out.Write(L"OnePiece", reinterpret_cast<const std::vector<int>&>(OnePiece)); 
					out.Write(L"EquipLv", (int)EquipLv);
					out.Write(L"CooldownCat", (int)CooldownCat);
					out.Write(L"Cooldown", Cooldown);
					out.Write(L"Limits", Limits);
					out.Write(L"States", States);
					out.Write(L"StateBundleConditions", StateBundleConditions);
					out.Write(L"AnimationPath", AnimationPath);
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"EffectCategory", EffectCategory);
					out.Write(L"EffectPath", EffectPath);
					out.Write(L"ItemEffectName", ItemEffectName);
					out.Write(L"ItemEffectGroup", ItemEffectGroup); // 3.1 by ultimate
					out.Write(L"MeshName", MeshName);
					out.Write(L"MeshGroup", MeshGroup); // 3.1 by ultimate
					out.Write(L"IconName", IconName);
					out.Write(L"IconGroup", IconGroup); // 3.1 by ultimate
					out.Write(L"Description", Description);
					out.Write(L"Scale", Scale);
					out.Write(L"ComposeData", ComposeData); // Watch out for this :) Teles 
				}

				void ItemInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::ItemInfo");
					in.Read(L"Id", Id);
					Hash = StringUtil::Hash(Id.c_str());
					in.Read(L"ShopType", (int&)ShopType, 0);
					in.Read(L"Sellers", Sellers, std::vector<std::wstring>());
					in.Read(L"SellCategory", SellCategory, 0);
					in.Read(L"Price", Price);
					in.Read(L"SellPrice", (int&)SellPrice, 0);
					in.Read(L"SellCount", (int&)SellCount, 1);
					in.Read(L"LadderPoint", LadderPoint, (uint32)0);
					in.Read(L"MarketCategory", (int&)MarketCategory, 0);
					in.Read(L"ItemType", (int&)ItemType);
					in.Read(L"MaxCnt", MaxCnt);
					in.Read(L"MaxUse", MaxUse, (uint8)1);
					in.Read(L"Attributes", Attributes);
					in.Read(L"EquipParts", (int&)EquipParts);
					in.Read(L"OnePiece", reinterpret_cast<std::vector<int>&>(OnePiece), std::vector<int>());
					in.Read(L"EquipLv", (int&)EquipLv);
					in.Read(L"CooldownCat", (int&)CooldownCat);
					in.Read(L"Cooldown", Cooldown);
					in.Read(L"Limits", Limits);
					in.Read(L"States", States);
					in.Read(L"StateBundleConditions", StateBundleConditions, StateBundleInfo::ConditionList());
					in.Read(L"AnimationPath", AnimationPath);
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"EffectCategory", EffectCategory, std::wstring());
					in.Read(L"EffectPath", EffectPath, std::wstring());
					in.Read(L"ItemEffectName", ItemEffectName, std::wstring());
					in.Read(L"ItemEffectGroup", ItemEffectGroup, std::wstring()); // 3.1 by ultimate
					in.Read(L"MeshName", MeshName, std::wstring());
					in.Read(L"MeshGroup", MeshGroup, std::wstring()); // 3.1 by ultimate
					in.Read(L"IconName", IconName, std::wstring());
					in.Read(L"IconGroup", IconGroup, std::wstring()); // 3.1 by ultimate
					in.Read(L"Description", Description, std::wstring());
					in.Read(L"Scale", Scale, float3(1, 1, 1));
					in.Read(L"ComposeData", ComposeData, ComposeData::ComposeData()); // Watch out for this :) Teles 
				}

				void ItemInfo::ComposeData::Serialize(Serializer::IStreamWriter& out) const {
					out.Begin(L"AllM::XRated::Database::Info::ItemInfo::ComposeData");
					out.Write(L"Type", Type);
					out.Write(L"GradePoint", GradePoint);
					out.Write(L"TypePoint1", TypePoint1);
					out.Write(L"TypePoint2", TypePoint2);
					out.Write(L"TypePoint3", TypePoint3);
				}

				void ItemInfo::ComposeData::Deserialize(Serializer::IStreamReader& in) {
					in.Begin(L"AllM::XRated::Database::Info::ItemInfo::ComposeData");
					in.Read(L"Type", Type, uint8(0));
					in.Read(L"GradePoint", GradePoint, uint16(0));
					in.Read(L"TypePoint1", TypePoint1, uint8(0));
					in.Read(L"TypePoint2", TypePoint2, uint8(0));
					in.Read(L"TypePoint3", TypePoint3, uint8(0));
				}

				void SetItemInfo::SetEffect::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SetItemInfo::SetEffect");
					out.Write(L"PieceCnt", PieceCnt);
					out.Write(L"States", States);
					out.Write(L"StateBundleConditions", StateBundleConditions);
				}

				void SetItemInfo::SetEffect::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SetItemInfo::SetEffect");
					in.Read(L"PieceCnt", PieceCnt, (uint8)0);
					in.Read(L"States", States, std::vector<ActionInfo::State>());
					in.Read(L"StateBundleConditions", StateBundleConditions, StateBundleInfo::ConditionList());
				}


				void SetItemInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SetItemInfo");
					out.Write(L"Items", Items);
					out.Write(L"SetEffects", SetEffects);
				}

				void SetItemInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SetItemInfo");
					in.Read(L"Items", Items);
					in.Read(L"SetEffects", SetEffects, SetEffects);
				}

			}
		}
	}
}