#pragma once
#include <LZMA/LzmaLib.h>
#include "CompressedActionsManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void CompressedActionInfoManager::Actions::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::CompressedActionInfoManager::Actions");
					out.Write(L"compressedActionMap", compressedActionMap);
					out.Write(L"reverseMap", reverseMap);
					out.Write(L"actorList", actorList);

				}

				void CompressedActionInfoManager::Actions::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::CompressedActionInfoManager::Actions");
					in.Read(L"compressedActionMap", compressedActionMap);
					in.Read(L"reverseMap", reverseMap);
					in.Read(L"actorList", actorList);
					for (auto y : actorList) {
						std::wcout << L"actorList: " << y.first.c_str() << std::endl;
						for (auto x : compressedActionMap.dataPosition) {
							std::wcout << "		name: " << x.first.c_str() << L"  - pos: " << x.second << std::endl;
						}
					}
				}

				void CompressedActionInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/CompressedActionInfos.b");
					std::map<std::wstring, Actions> compressedActionMap;
					reader->Read(L"compressedActionMap", compressedActionMap);
				}

				ActionInfoManager::Actions& CompressedActionInfoManager::Retrieve(const int templateOffset)
				{
					Resource::StreamReader cbfreader = Resource::ResourceSystemInstance().CreateStreamReader(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug\\Database\\ActionInfos.cbf");
					cbfreader->SetReadCursor(templateOffset, Lunia::IStream::Begin);
					/* CompressedBlockSizeInBytes */
					printf("Decimal: %d - ", cbfreader->GetReadCursor());
					std::cout << "Offset: " << std::hex << cbfreader->GetReadCursor();
					uint8* Buffer = reinterpret_cast<uint8*>(new char[4]);
					cbfreader->Read(Buffer, 4);
					/* BlockLength */
					size_t compSize = *(int*)Buffer;
					printf(" - Size : (Decimal: %d, Hex: 0%.2x) \n", compSize, compSize);
					/* BlockRead */
					uint8* Block = reinterpret_cast<uint8*>(new char[compSize]);
					cbfreader->Read(Block, compSize);
					/* Block Reader */
					Resource::StreamReader reader = new FileIO::RefCountedMemoryStreamReader(Block, compSize);
					uint32 count(0);
					/* Loop in Block Size Left */
					while (reader->GetSizeLeft() > 0)
					{
						/* Reading and setting a first block data in ReplayBuffer*/
						reader->Read(Block, 4);
						size_t srcSize = *(int*)Block + LZMA_PROPS_SIZE;
						reader->Read(Block, 4);
						uint32 UNCOMPRESSED_SIZE = *(int*)Block;
						uint8* lReplayBuffer = reinterpret_cast<uint8*>(new char[srcSize]);
						reader->Read(lReplayBuffer, srcSize);

						/* Setting buffer input and output sizes*/
						std::vector<uint8> inBuf(srcSize);
						std::vector<uint8> outBuf;
						outBuf.resize(UNCOMPRESSED_SIZE);
						memcpy(&inBuf[0], lReplayBuffer, srcSize);

						/*decoding and decrypting the binary owo*/
						size_t dstLen = outBuf.size();
						size_t srcLen = inBuf.size() - LZMA_PROPS_SIZE;
						SRes res = LzmaUncompress(&outBuf[0], &dstLen, &inBuf[LZMA_PROPS_SIZE], &srcLen, &inBuf[0], LZMA_PROPS_SIZE);

						Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&outBuf[0], compSize));
						//BlockDecrypted->Read(L"actionMap", actionMap);
					}
				}
			}
		}
	}
}