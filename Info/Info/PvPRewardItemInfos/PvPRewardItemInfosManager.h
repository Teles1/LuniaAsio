#pragma once
#include "PvPRewardInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class PvPRewardItemTable
				{
					std::map< uint32 /*StageHash*/, Info::PvPRewardInfo > infos;

				public:
					bool GetRewardInfo(uint32 stageHash, Info::PvPRewardInfo& rewardInfo);

					void Load(bool xml);
					void Save(bool xml);
				};
			}
		}
	}
}
