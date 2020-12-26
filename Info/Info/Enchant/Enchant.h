#pragma once
#include <Core/FileIO/File.h>
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
#include <Info/Info/Actions/Actions.h>
#include <Info/Info/Items/Item.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct EnchantBitfields
				{
					uint64 
						Option1	: 4 ,
						Level1	: 3 ,
						Option2	: 4 ,
						Level2	: 3 ,
						Option3	: 4 ,
						Level3	: 3 ,
						Identified	: 1 ,

					/* 3 bits */
						LightLevel : 3,

					/* 4 bits */
						Reinforcement : 4 ,

					/* 18 bits  */
						Magic1 : 9 ,
						Magic2 : 9 ,

						DestroyedByReinforcement : 1,
						DestroyedByLightReinforcement : 1,

						HasEquipped : 1,

						RestoreBelongingTimes : 2,
			
						NormalIdentified : 1,

						ExtraField : 11;

					/* helpers */
					operator int64 () const { return reinterpret_cast<const int64&>(*this); }
					EnchantBitfields& operator =(int64 rhs) { reinterpret_cast<int64&>(*this)=rhs; return *this; }
					EnchantBitfields() { *this=0; }
					EnchantBitfields(InstanceEx rhs){
						*this=rhs.Instance;
						/*
							PLEASE. NEVER DO THE SAME FOR NORMALBITFIELD. This conversion is allowed hence there is no expiration for EnchantItens anyways.
						*/
					} 
					EnchantBitfields(int64 rhs) { *this=rhs; }
					bool IsDestoryed() const { return (DestroyedByReinforcement || DestroyedByLightReinforcement); }

					std::pair<uint8/*option*/, uint8/*level*/> GetIdentifyFields(int index) const; ///< @param must be 0~2
					std::pair<uint16/*first*/, uint16/*second*/> GetMagicalAttach() const;

					/* validator */
					~EnchantBitfields()
					{
					}
				};

				struct NormalBitfields
				{
					uint64
			
					/* Cash Enchant fields */
						CashEnchant1	: 11, // 0,11
						CashEnchant2	: 11, // 12,11
						CashEnchant3	: 11, // 23,11
						CashEnchant4	: 11, // 34,11

						HasEquipped		: 1, // 46,1
			
						RestoreBelongingTimes : 2, //47,2
				
						Reserved		: 17;

					operator int64 () const { return reinterpret_cast<const int64&>(*this); }
					NormalBitfields& operator =(int64 rhs) { reinterpret_cast<int64&>(*this)=rhs; return *this; }
					NormalBitfields() { *this=0; }
					NormalBitfields(int64 rhs) { *this=rhs; }

					/* bool IsExpired(const DateTime& now=DateTime::Now()) const;
					void ForceExpiration();
					void MakeUnlimitedPeriod();
					DateTime GetExpiredDate() const;
					DateTime ExtensionExpiredDay(uint32 day); */

					/* validator */
					~NormalBitfields()
					{
					}
				};


				//���� ���� ����� ����
				void GetIdentifyResults( std::vector< std::vector<float> >& results , const Info::ItemInfo* info, EnchantBitfields enchant );

				/* enchant ������ ������ ItemInfo �ν��Ͻ� ������ */
				void MakeReinforcementParam(const Info::ItemInfo* info, EnchantBitfields enchant, Info::ItemInfo& resultInfo, float basisRatioValue);
				void MakeLightReinforcementParam(const Info::ItemInfo* info, EnchantBitfields enchant, Info::ItemInfo& resultInfo, float basisRatioValue);

				Info::ItemInfo GetItemInstance(const Info::ItemInfo* info, EnchantBitfields enchant);
				bool GetItemInstanceForServer(const Info::ItemInfo* info, EnchantBitfields enchant, std::vector<Info::ActionInfo::State>& states);
		
				InstanceEx MakeInstance( const Database::Info::ItemInfo* itemInfo, uint16 cashEnchant1Index, uint16 cashEnchant2Index, uint32 expireMin, bool addExpireMin, InstanceEx pInstance, uint16 cashEnchant3Index = 0, uint16 cashEnchant4Index = 0 );
				//std::wstring GetTimeStringFromInstance( InstanceEx instance, const Lunia::DateTime& currentTime );
		
				bool IsEnchantable(const Info::ItemInfo* info);
				bool IsCashEnchantable(const Info::ItemInfo& info);

				bool IsRestorableBelonging(const Info::ItemInfo* info, const int64 instance);
				bool RestoreBelonging(const Info::ItemInfo* info, InstanceEx& instance);
				int GetItemRestoreBelongingTimes(const Database::Info::ItemInfo* info,const int64 instance);
	
				struct RequiredItem : public Serializer::ISerializable {
					uint32 ItemHash;
					uint16 RequireCount;
					RequiredItem() : ItemHash(0), RequireCount(0) {}
					RequiredItem(uint32 hash, uint16 cnt) : ItemHash(hash), RequireCount(cnt) {}
					bool operator <(const RequiredItem& rhs) const { if (ItemHash==rhs.ItemHash) return (RequireCount<rhs.RequireCount); return ItemHash<rhs.ItemHash; }
				public : // Serializer::ISerializable impl
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};

				struct Condition {
					uint32 Fee;
					std::vector<RequiredItem> reqiredItems;
					Condition() : Fee(0) {}
				};

				//Requirement functions
				//����( �˻��� �±� ) ������� Ŭ������ �����־ �̷� �ּ� ������ �ʿ� ����� �Ѵ�.
				uint32 GetIdentifyRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				Condition GetReinforcementRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				Condition GetLightReinforcementRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				Condition GetMagicalAttachRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				Condition GetAlchemyRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				uint32 GetExtractionRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				Condition GetRestoreStrengthenRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				Condition GetRestoreAppraisalRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				Condition GetRestoreBelongingRequirement(const Info::ItemInfo* info, const EnchantBitfields& enchant);
				//struct Type {
				//	const static int Identify		= 0;
				//	const static int Reinforce		= 1;
				//	const static int LightReinforce	= 2;
				//	const static int MagicalAttach	= 3;
				//};
				//typedef int EnchantType;

				inline float GetDmgByLightReinforcement(int lv) { return (lv*lv + lv)*0.25f + 0.5f; }
				inline float GetStatByLightReinforcement(int lv) { return (lv*lv + lv)*0.5f + 1; }
				inline float Get2ndStatByLightReinforcement(int lv) { return GetStatByLightReinforcement(lv)*3; }
				inline float GetResistByLightReinforcement(int lv) { return (float)lv; }


				struct DailyItemLastAcquiredDate
				{
					uint32
						Year	: 11,
						Month	: 4,
						Day		: 5,

						/** ���� �κ� */
						Reserved :12;

				};
				bool IsHaveTodayDailyItem(const Lunia::DateTime::Date& connectDate,InstanceEx instance);
				bool IsExpired(const Database::Info::ItemInfo* info, InstanceEx instance);
			}
		} 
	} 
}
