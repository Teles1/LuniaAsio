#ifndef GuildInfo_H
#define GuildInfo_H
#pragma once
#ifndef Serializer_H
#include <Core/Serializer/Serializer.h>
#endif // !Serializer_H
#ifndef CashShopBarter_H
#include <Info/Info/CashShopBarter/CashShopBarter.h>
#endif // !CashShopBarter_H

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct GuildInfo {
					struct General {
						typedef std::vector<uint32> LevelUpExpList;
						LevelUpExpList	LevelUpExps;
						uint32 MinuteForExp;

						std::vector<uint32>			MaxGuildPoints;
						std::vector<float>			DiscountRate;
						std::vector<uint32>			MaxGuildUser;

						Database::Info::ShopInfo::CategoryTabInfo	GuildCashItemTabInfo;
						uint32	CreationFee;			
						int MinimalCreationPlayerLevel;	
						uint32	LeaveFee;				
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					struct Level {
						float		AddExpFactor;							
						ShopInfo::CategoryList		ShopCategorys;
						/** std::pair<uint32, std::wstring>
							first : passive hash.
							second : passive discription
						*/
						typedef std::vector< std::pair<uint32, std::wstring> >	GuildPassiveList;
						GuildPassiveList										GuildPassives;
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					struct Rank
					{
						ShopInfo::CategoryList		ShopCategorys;
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
				};
			}
		}
	}
}
#endif // ! GuildInfo_H