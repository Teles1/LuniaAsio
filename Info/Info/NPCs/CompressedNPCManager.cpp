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

					compressedNpcCbf = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/NPCInfos.cbf");

				}
					
				void CompressedNPCInfoManager::BinaryToXml() {
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"CompressedNPCInfos.xml");
					writer->Write(L"compressedNpcs", compressedNpcs);
				}

				void CompressedNPCInfoManager::GetNPC(const uint32 templateOffset)
				{
					compressedNpcCbf->SetReadCursor(templateOffset + 4, Lunia::IStream::Begin);
					/* CompressedBlockSizeInBytes */
					uint8* SizeBlock = reinterpret_cast<uint8*>(new char[4]);

					/* Reading and setting a first block data in ReplayBuffer*/
					compressedNpcCbf->Read(SizeBlock, 4);
					size_t srcSize = *(int*)SizeBlock + LZMA_PROPS_SIZE;
					compressedNpcCbf->Read(SizeBlock, 4);
					uint32 UNCOMPRESSED_SIZE = *(int*)SizeBlock;
					uint8* lReplayBuffer = reinterpret_cast<uint8*>(new char[srcSize]);
					compressedNpcCbf->Read(lReplayBuffer, srcSize);

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
					BlockDecrypted->Read(L"NPCInfoManager", Npcs, false);
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

				NPCInfoManager::BasicResist::Resist* CompressedNPCInfoManager::Retrieve(NonPlayerInfo::Races race)
				{
					if (race < 0 || race > 10) return NULL;
					return &basicResist.resist[race];
				}
			}
		}
	}
}