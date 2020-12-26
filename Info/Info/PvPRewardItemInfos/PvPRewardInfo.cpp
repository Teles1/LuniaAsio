#pragma once
#include "PvPRewardInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				PvPRewardInfo::RewardItemInfo::RewardItemInfo()
					: Hash(0), Count(0)
				{
				}

				void PvPRewardInfo::RewardItemInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"PvPRewardInfo::RewardItemInfo");

					out.Write(L"Hash", Hash);
					out.Write(L"Count", Count);
				}

				void PvPRewardInfo::RewardItemInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"PvPRewardInfo::RewardItemInfo");

					in.Read(L"Hash", Hash);
					in.Read(L"Count", Count);
				}


				PvPRewardInfo::PvPRewardInfo()
				{
				}

				void PvPRewardInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"PvPRewardInfo");

					out.Write(L"WinnerItem", WinnerItem);
					out.Write(L"LoserItem", LoserItem);
				}

				void PvPRewardInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"PvPRewardInfo");

					in.Read(L"WinnerItem", WinnerItem);
					in.Read(L"LoserItem", LoserItem);
				}
			}
		}
	}
}
