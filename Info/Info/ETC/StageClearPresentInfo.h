#ifndef StageClearPresentInfo_H
#define StageClearPresentInfo_H
#pragma once
#ifndef FamilyInfo_H
#include <Info/Info/GuildFamily/FamilyInfo.h>
#endif // !FamilyInfo_H

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct StageClearPresentInfo
				{
					uint16	RegularMemberOnlineMinCount;
					uint16	GuestMemberOnlineMinCount;
					FamilyPresentInfo		Present;
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
#endif StageClearPresentInfo_H // !StageClearPresentInfo_H