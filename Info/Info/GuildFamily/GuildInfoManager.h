#ifndef GuildInfoManager_H
#define GuildInfoManager_H
#pragma once
#ifndef DateTime_H
#include<Core/Utils/DateTime.h>
#endif // !DateTime_H

#ifndef GuildInfo_H
#include<Info/Info/GuildFamily/GuildInfo.h>
#endif // !GuildInfo_H

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class GuildInfoManager
				{
					GuildInfo::General						general;

					typedef std::vector<GuildInfo::Level>	LevelInfoList;
					LevelInfoList							levelInfos;

					typedef std::map<uint32/*rank*/, GuildInfo::Rank>	RankInfoList;
					RankInfoList							rankInfos;
				private:
					// using only server side validate(level to buy validate).
					typedef std::vector<uint32>				SellItemLists;
					typedef std::vector< SellItemLists >	LevelToSellItemList;
					LevelToSellItemList						levelToSellItems;

					typedef std::map<uint32/*rank*/, SellItemLists >	RankToSellItemList;
					RankToSellItemList						rankToSellItems;
				public:
					const GuildInfo::Level* GetLevelInfo(uint8 level) const;
					const GuildInfo::Rank* GetRankInfo(uint32 rank) const;
					const GuildInfo::General& GetGeneral() const;
					/** GetNextLevelExp.
						@return first : true(have),false(have not), second : need nextLevelExp.
					*/
					uint32 GetMaxGuildPoint(uint8 currentLevel) const;
					std::pair<bool, uint32> GetNextLevelExp(uint8 currentLevel) const;
					uint32 GetMaxGuildUserPerLevel(uint8 currentLevel) const;
					DateTime GetGuildUserExpAddTime(DateTime startTime, float userExp) const;
					float GetShopDiscountRate(uint16 rank) const;

				public:
					/** CalculdateCurrentUserExp.
						@return first :current Exp, added guild exp
					*/
					std::pair<float, uint32> CalculdateCurrentUserExp(DateTime start, DateTime currentTime, float oldPlayTime) const;
					static float CalculatePlayTime(DateTime startTime, DateTime now, float oldPlayTime);

				public:
					/** SortSellItemList.
						@bref : shop item list  sort to level to sell item list(origin date is sort to category/with visual).
					*/
					void SortSellItemList();
					/** IsShopItem.
						@bref : guild level to item validater
					*/
					bool IsShopItem(uint8 guildLevel, uint32 guildRank, uint32 hash) const;
				public:
					void Load(bool xml);
					void Save(bool xml);
					void Init(bool xml);
				};
			}
		}
	}
}
#endif //!GuildInfoManager_H