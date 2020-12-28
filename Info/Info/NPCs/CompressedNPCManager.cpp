#pragma once
#include "CompressedNPCManager.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void CompressedNPCInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/CompressedNPCInfos.b");
					reader->Begin(L"AllM::XRated::Database::Info::NPCInfoManager");
					reader->Read(L"compressedNpcs", compressedNpcs);
				}
					
				void CompressedNPCInfoManager::BinaryToXml() {
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"CompressedNPCInfos.xml");
					writer->Write(L"compressedNpcs", compressedNpcs);
				}

				void CompressedNPCInfoManager::GetNPC(const uint32 templateOffset)
				{
					Resource::StreamReader cbfreader = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/NPCInfos.cbf");
					cbfreader->SetReadCursor(0, Lunia::IStream::Begin);
					/* CompressedBlockSizeInBytes */
					for (int i = 0; i < 30000; i++) {

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
						int32 blocksReaded = 0;
						completeBuff.resize(4);
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

							//if do you want save all outbuf in a vector uncomment this and good luck <3
							blocksReaded += *(int*)(&outBuf[0]);
							outBuf.erase(outBuf.begin(), outBuf.begin() + 4);
							std::move(outBuf.begin(), outBuf.end(), std::back_inserter(completeBuff));
							completeSize += dstLen;
						}
					}
					//Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&outBuf[0], dstLen));
					//BlockDecrypted->Read(L"NPCInfoManager", Npcs, false);
				}

				NonPlayerInfo* CompressedNPCInfoManager::Retrieve(const uint32 hash) {
					if (compressedNpcs.dataPosition.find(hash) == compressedNpcs.dataPosition.end()) return nullptr;
					if (this->Npcs.find(hash) != this->Npcs.end()) return &Npcs[hash];
					GetNPC(compressedNpcs.dataPosition[hash]);
					return &Npcs[hash];
				}

				NonPlayerInfo* CompressedNPCInfoManager::Retrieve(const wchar_t* name) {
					return Retrieve(StringUtil::Hash(name));
				}
			}
		}
	}
}