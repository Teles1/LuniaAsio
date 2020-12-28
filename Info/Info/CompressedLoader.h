#ifndef CompressedLoader_H
#define CompressedLoader_H

#pragma once
#include <Core/Resource/Resource.h>
#include <LzmaLib/LzmaLib.h>
#include <iostream>
#include <Core/Serializer/Binary/BinaryStream.h>
#include <Core/FileIO/FileStream.h>
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


					/*
						[0-3] 	Bytes that belong to the object being read
							First item
							[4-7] 	compressed size
							[8-11] 	uncompressed size
							[11-12]	Prop
					*/
				};
			}
		}
	}
}

#endif