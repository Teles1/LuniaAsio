#pragma once
#include <LZMA/LzmaLib.h>
#include "CompressedActionsManager.h"
#include <iostream>
#include <fstream>

std::vector<std::wstring> populated_data;

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				//std::wofstream outfile("C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug\\CompressedActionsManager.txt");
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

					//if (outfile.is_open()) { // generating a txt to debug
						for (auto y : actorList) {
							populated_data.push_back(y.first.c_str());
							//outfile << L"actorList:" << (L"%s", y.first.c_str()) << std::endl;
							//for (auto x : compressedActionMap.dataPosition) {
							//	//std::wcout << L"		name: " << x.first.c_str() << L"  - pos: " << x.second << std::endl;
							//	outfile << L"		name: " << (L"%s", x.first.c_str()) << L"  - pos: " << x.second << std::endl;
							//}
						}
					//}
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
					std::vector<uint8> completeBuff;
					size_t completeSize = 0;
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

						Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&outBuf[0], dstLen));
						ActionInfoManager::Actions a;
						BlockDecrypted->Read(L"ActionMap", a);

						//if do you want save all outbuf in a vector uncomment this and good luck <3
						//std::move(outBuf.begin(), outBuf.end(), std::back_inserter(completeBuff));
						//completeSize += dstLen;
						//Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&completeBuff[0], completeSize));

					}
				}
			}
		}
	}
}