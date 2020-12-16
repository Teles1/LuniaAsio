#ifndef CompressedLoader_H
#define CompressedLoader_H

#pragma once
#include <Core/Resource/Resource.h>
#include <Core/Serializer/Binary/BinaryStream.h>
#include <Core/FileIO/FileStream.h>
#include <Lzma/LzmaLib.h>
#include <iostream>

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
					inline void Read(const String& name, T& data) {
						/*
							[0-3] 	Bytes that belong to the object being read
								First item
								[4-7] 	compressed size
								[8-11] 	uncompressed size
								[11-12]	Prop
						*/

						uint8* Buffer = reinterpret_cast<uint8*>(new char[4]);
						cbfreader->Read(Buffer, 4);
						size_t compSize = *(size_t*)Buffer;
						Buffer = reinterpret_cast<uint8*>(new char[compSize]);
						cbfreader->Read(Buffer, compSize);
						Lunia::Resource::StreamReader reader = new Lunia::FileIO::RefCountedMemoryStreamReader(Buffer, compSize);
						while (reader->GetSizeLeft() > 0)
						{
							/* printf("offset[%d]\n", reader->GetReadCursor()); */
							reader->Read(Buffer, 4);
							size_t srcSize = *(size_t*)Buffer + LZMA_PROPS_SIZE;

							reader->Read(Buffer, 4);
							uint32 UNCOMPRESSED_SIZE = *(size_t*)Buffer;
							//std::cout << "srcSize[" << srcSize << "], UNCOMPRESSED_SIZE[" << UNCOMPRESSED_SIZE << "]" << std::endl;
							//just to make sure that this buffer will be clean I'm making it be declared every time.
							uint8* lReplayBuffer = reinterpret_cast<uint8*>(new char[srcSize]);
							reader->Read(lReplayBuffer, srcSize);
							//reader->EndOfStream();

							std::vector<uint8> inBuf(srcSize), outBuf;
							outBuf.resize(UNCOMPRESSED_SIZE);

							memcpy(&inBuf[0], lReplayBuffer, srcSize);

							UnCompress(outBuf, inBuf);
							/* for(int i = 0; i < UNCOMPRESSED_SIZE; i++){
								//printf("%.2X ", outBuf[i]);
							} */
							//std::cout << "UnCompressed" << std::endl;
							Lunia::Resource::SerializerStreamReader binary =
								Serializer::CreateBinaryStreamReader(
									new Lunia::FileIO::RefCountedMemoryStreamReader(&outBuf[0], UNCOMPRESSED_SIZE));
							binary->Read(name.c_str(), data, false);

							//std::cout << "UpgradeTables Loaded ..." << std::endl;
						}
						/* Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"./Database/UpgradeTables.xml");
						writer->Write(L"UpgradeTables", UpgradeTables); */
						std::wcout << name.c_str() << L" Loaded ..." << std::endl;
					}
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

#endif