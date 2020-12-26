#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct StagePlayTime {
					uint32	AveragePlayTime;
					float StandardDiviation;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}