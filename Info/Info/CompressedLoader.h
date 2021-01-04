#ifndef CompressedLoader_H
#define CompressedLoader_H
#pragma once
#include <Core/Serializer/Serializer.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				template <typename Template>
				struct PositionData : public Serializer::ISerializable
				{
					Template dataPosition;
					virtual void Serialize(Serializer::IStreamWriter& out) const
					{
						out.Write(L"dataPosition", dataPosition);
					}
					virtual void Deserialize(Serializer::IStreamReader& in)
					{
						in.Read(L"dataPosition", dataPosition);
					}
				};
			}
		}
	}
}

#endif