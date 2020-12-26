#ifndef PvPRankRewardItemTable_H
#define PvPRankRewardItemTable_H
#pragma once
#include "PvPRankRewardInfo.h"
#include <map>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class PvPRankRewardItemTable
				{
					std::map< uint32 /*Rank*/, Info::PvPRankRewardInfo > infos;

				public:
					bool GetRewardInfo(uint32 pvpRank, Info::PvPRankRewardInfo& rewardInfo);

					void Load(bool xml);
					void Save(bool xml);
				};
			}
		}
	}
}
#endif // PvPRankRewardItemTable_H