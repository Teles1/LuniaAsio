#pragma once
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				class CompressedLoader {
				public:
					Lunia::Resource::StreamReader cbfreader;
					String filename;
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

					int UnCompress(std::vector<unsigned char>& outBuf, const std::vector<unsigned char>& inBuf);
					String GetStructName();
					template <typename T>
					void Read(const String& name, T& data);
				};
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