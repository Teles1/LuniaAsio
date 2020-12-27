#include "PvPRankRewardInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void PvPRankRewardInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::PvPRankRewardInfo");
					out.Write(L"StateBundleConditions", StateBundleConditions);
				}

				void PvPRankRewardInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::PvPRankRewardInfo");
					in.Read(L"StateBundleConditions", StateBundleConditions, StateBundleInfo::ConditionList());
				}
			}
		}
	}
}