#pragma once
#include <Info/Info/StateBundles/StateBundles.h>
#include <Info/Info/Actions/Actions.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct ItemCategory : public Serializer::ISerializable {
					enum CategoryType { NOCATEGORY = 0, HPPOTION, MPPOTION, CATEGORYCNT };
					CategoryType categoryType;
					float cooldown;
				public: /* ISerializable implementation */
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				struct ItemInfo : public Serializer::ISerializable
				{
					class IFindCondition
					{
					public: virtual bool operator()(const ItemInfo& rhs) const = 0;
					};

					struct ATTRIBUTES {
						const static uint32 TRADE = 1 << 0;
						const static uint32 TRADEAFTERUSE = 1 << 1;
						const static uint32 CONSUME = 1 << 2;
						const static uint32 CASH = 1 << 3;
						const static uint32 SAVE = 1 << 4;
						const static uint32 USE = 1 << 5;
						const static uint32 ENCHANTPARTS = 1 << 6;
						const static uint32 ACQUIREBYTURN = 1 << 7;	
						const static uint32 STAGEITEM = 1 << 8;
						const static uint32 USEEX = 1 << 9;	
						const static uint32 PASSIVE = 1 << 10;	
						const static uint32	USEWHENGHOST = 1 << 11;	
						const static uint32	EXISTWHENEXPIRED = 1 << 12;	
						const static uint32 TRADEWHENEXPIRED = 1 << 13;	
						const static uint32 GUILDITEM = 1 << 14;	
						const static uint32 ACQUIREBYDICE = 1 << 15;   
						const static uint32 ADDINQUICKSLOT = 1 << 16;   
					};

					enum Type {
						EQUIPMENT = 0,	
						INSTANT,		
						QUEST,			
						REAGENT,		
						MISC,			
						JEWEL,			
						UNIDENTIFIED,	
						PVPITEM,		
						ENCHANTSCROLL,	
						PETEQUIPMENT,
						COMBOITEM = 12,		

						END_OF_ITEM_TYPE
					};

					struct EquipmentLevel {
						enum Type { Low = 0, Normal, Rare, Epic, Legend, Artifect, Avata, };
					};

					std::wstring Id;
					uint32 Hash;

					
					Constants::ShopType ShopType; 
					std::vector<std::wstring> Sellers;
					int SellCategory;
					uint32 Price;
					uint32 SellCount;
					uint32 SellPrice;
					uint32 LadderPoint;

					uint32 MarketCategory;

					Type ItemType;
					int MaxCnt;	
					uint8 MaxUse; 

					typedef uint32 RetTypeOfAttribute;
					struct Attribute : public Serializer::ISerializable { 
						RetTypeOfAttribute Attr;	

					public:
						Attribute() : Attr(0) {}
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					} Attributes;

					
					Constants::EquipParts EquipParts;		
					std::vector<Constants::EquipParts> OnePiece; 

					EquipmentLevel::Type EquipLv;	

					
					ItemCategory::CategoryType CooldownCat;
					float Cooldown;

					struct Limit
						: public Serializer::ISerializable
					{
						Constants::ClassType Class;
						int MajorStat, Level, MaxLevel, PvpLevel, MaxPvpLevel, WarLevel, MaxWarLevel, StoredLevel, MaxStoredLevel, RebirthCount, MaxRebirthCount;
						int Str, Int, Dex, Vit;
						std::vector< std::wstring > StageIds;
						std::vector< std::wstring > UnableStageIds;

						

						uint32 StageType; 
						bool BFatal; 

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					} Limits;

					
					std::vector<ActionInfo::State> States;
					StateBundleInfo::ConditionList StateBundleConditions;

					
					std::wstring AnimationPath;	
					std::wstring DisplayName;
					std::wstring EffectCategory;
					std::wstring EffectPath;
					std::wstring ItemEffectName;
					std::wstring ItemEffectGroup;
					std::wstring MeshName;
					std::wstring MeshGroup;
					std::wstring IconName;
					std::wstring IconGroup;
					std::wstring Description;
					float3 Scale;
					struct ComposeData : public Serializer::ISerializable {
						uint8 Type;
						uint16 GradePoint;
						uint8 TypePoint1;
						uint8 TypePoint2;
						uint8 TypePoint3;
					public: 
						ComposeData() : Type(0), GradePoint(0), TypePoint1(0), TypePoint2(0), TypePoint3(0) {}
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					}ComposeData;

					
					


				public:
					ItemInfo() { }
					~ItemInfo() { }

					const RetTypeOfAttribute& GetAttributes() const { return Attributes.Attr; }
				public: 
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
				
				struct SetItemInfo : public Serializer::ISerializable {
					struct SetEffect : public Serializer::ISerializable {
						uint8 PieceCnt;
						std::vector<ActionInfo::State> States;
						StateBundleInfo::ConditionList StateBundleConditions;

					public: /* ISerializable implementation */
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					std::vector<uint32> Items;
					std::vector<SetEffect> SetEffects;

				public:
					uint32 GetId() const { return Items.front(); }
					bool operator== (uint32 itemHash) const { return Items.front() == itemHash; }

				public: /* ISerializable implementation */
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}