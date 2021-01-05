#include "SwapSetInfo.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void SwapSetInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SwapSetInfo");
					out.Write(L"SetIndex", SetIndex);
					out.Write(L"BagNumber", BagNumber);
					out.Write(L"EquipSetName", EquipSetName);
				}

				void SwapSetInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SwapSetInfo");
					in.Read(L"SetIndex", SetIndex);
					in.Read(L"BagNumber", BagNumber);
					in.Read(L"EquipSetName", EquipSetName);
				}
			}
		}
	}
}