#pragma once
#include "StagePlayTime.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void StagePlayTime::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StagePlayTimeInfo");
					out.Write(L"AveragePlayTime", AveragePlayTime);
					out.Write(L"StandardDiviation", StandardDiviation);
				}
				void StagePlayTime::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StagePlayTimeInfo");
					in.Read(L"AveragePlayTime", AveragePlayTime);
					in.Read(L"StandardDiviation", StandardDiviation);
				}
			}
		}
	}
}