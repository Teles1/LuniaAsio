#pragma once
#include "StagePlayTime.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class StagePlayTimeInfoManager
				{
					typedef std::map<uint32, StagePlayTime, Less<uint32> > StagePlayTimeContainer;
					StagePlayTimeContainer stagePlayTimes;
				public:
					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();

					virtual StagePlayTime* Retrieve(uint32 stageHash);
					virtual void SetStagePlayTime(uint32 hash, StagePlayTime& data);
				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
