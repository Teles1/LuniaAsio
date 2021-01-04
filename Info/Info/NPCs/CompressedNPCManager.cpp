#pragma once
#include "CompressedNPCManager.h"
#include <LzmaLib/LzmaLib.h>
#include <Core/FileIO/FileStream.h>
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
					uint8* buffer = new uint8[4];

					/* Reading and setting a first block data in ReplayBuffer*/
					compressedNpcCbf->Read(buffer, 4);
					size_t srcSize = (size_t)(*(int*)buffer) + LZMA_PROPS_SIZE;
					compressedNpcCbf->Read(buffer, 4);
					uint32 UNCOMPRESSED_SIZE = *(int*)buffer;
					delete[] buffer;

					buffer = new uint8[srcSize];
					compressedNpcCbf->Read(buffer, (uint32)srcSize);

					/* Setting buffer input and output sizes*/
					std::vector<uint8> inBuf(srcSize);
					std::vector<uint8> outBuf;
					outBuf.resize(UNCOMPRESSED_SIZE);
					memcpy(&inBuf[0], buffer, srcSize);

					/*decoding and decrypting the binary owo*/
					size_t dstLen = outBuf.size();
					size_t srcLen = inBuf.size() - LZMA_PROPS_SIZE;
					SRes res = LzmaUncompress(&outBuf[0], &dstLen, &inBuf[LZMA_PROPS_SIZE], &srcLen, &inBuf[0], LZMA_PROPS_SIZE);

					Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(
						new FileIO::RefCountedMemoryStreamReader(&outBuf[0], (uint32)dstLen)
					);
					BlockDecrypted->Read(L"NPCInfoManager", Npcs, false);

					delete[] buffer;
				}

				NonPlayerInfo* CompressedNPCInfoManager::Retrieve(const uint32 hash) {
					if (compressedNpcs.dataPosition.find(hash) == compressedNpcs.dataPosition.end())
						return nullptr;
					if (this->Npcs.find(hash) != this->Npcs.end())
						return &Npcs[hash];
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