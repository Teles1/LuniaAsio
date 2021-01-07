#include "AwardInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				AwardInfo::AwardInfo()
				{
				}

				void AwardInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AwardInfo");
					out.Write(L"Items", Items);
					out.Write(L"Title", Title);
					out.Write(L"Text", Text);
					out.Write(L"Sender", Sender);
				}
				void AwardInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AwardInfo");
					in.Read(L"Items", Items);
					in.Read(L"Title", Title);
					in.Read(L"Text", Text);
					in.Read(L"Sender", Sender);
				}
			}
		}
	}
}