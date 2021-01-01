#include "StageClearPresentInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void StageClearPresentInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::StageClearPresentInfo");
					out.Write(L"RegularMemberOnlineMinCount", RegularMemberOnlineMinCount);
					out.Write(L"GuestMemberOnlineMinCount", GuestMemberOnlineMinCount);
					out.Write(L"Present", Present);
				}
				void StageClearPresentInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::StageClearPresentInfo");
					in.Read(L"RegularMemberOnlineMinCount", RegularMemberOnlineMinCount);
					in.Read(L"GuestMemberOnlineMinCount", GuestMemberOnlineMinCount);
					in.Read(L"Present", Present);
				}
			}
		}
	}
}
