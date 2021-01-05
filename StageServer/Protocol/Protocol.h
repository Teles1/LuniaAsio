#pragma once
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			namespace Protocol {
				class IPacketSerializable : public Serializer::ISerializable {
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const = 0;
					virtual void Deserialize(Serializer::IStreamReader& in) = 0;

					virtual const wchar_t* GetTypeName() const= 0;

					virtual ~IPacketSerializable() {}
				};
			}
		}
	}
}