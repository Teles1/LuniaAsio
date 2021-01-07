#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/Items/Item.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct AwardInfo : public Serializer::ISerializable
				{
				public:
					AwardInfo();
					std::vector< XRated::ItemBasicInfo > Items;
					std::wstring Title;
					std::wstring Text;
					std::wstring Sender;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);

				};
			}
		}
	}
}