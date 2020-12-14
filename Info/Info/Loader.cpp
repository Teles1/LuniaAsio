#include "Loader.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void FilePosData::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::FilePosData");
					out.Write(L"StartPos", StartPos);
					out.Write(L"EndPos", EndPos);
				}
				void FilePosData::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::FilePosData");
					in.Read(L"StartPos", StartPos);
					in.Read(L"EndPos", EndPos);
				}
			}
		}
	}
}