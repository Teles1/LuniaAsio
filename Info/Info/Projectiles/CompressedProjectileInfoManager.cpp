#pragma once
#include "CompressedProjectileInfoManager.h"
#include <LzmaLib/LzmaLib.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void CompressedProjectileInfoManager::Init()
				{
					Projectiles.clear();
					LoadBinaryData();
				}
				void CompressedProjectileInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/CompressedProjectileInfos.b");
					reader->Begin(L"AllM::XRated::Database::Info::ProjectileInfoManager");
					reader->Read(L"compressedProjectiles", compressedProjectiles);

					compressedProjectileCBF = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/ProjectileInfos.cbf");
				}

				void CompressedProjectileInfoManager::BinaryToXml() {
					/*Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"CompressedNPCInfos.xml");
					writer->Write(L"compressedNpcs", compressedNpcs);*/
				}

				void CompressedProjectileInfoManager::GetProjectile(const uint32 templateOffset)
				{
					compressedProjectileCBF->SetReadCursor(templateOffset + 4, Lunia::IStream::Begin);
					/* CompressedBlockSizeInBytes */
					uint8* Buffer = new uint8[4];

					/* Reading and setting a first block data in ReplayBuffer*/
					compressedProjectileCBF->Read(Buffer, 4);
					size_t srcSize = (size_t)(*(int*)Buffer) + LZMA_PROPS_SIZE;
					compressedProjectileCBF->Read(Buffer, 4);
					uint32 UNCOMPRESSED_SIZE = *(int*)Buffer;
					delete[] Buffer;

					Buffer = new uint8[srcSize];
					compressedProjectileCBF->Read(Buffer, (uint32)srcSize);

					/* Setting buffer input and output sizes*/
					std::vector<uint8> inBuf(srcSize);
					std::vector<uint8> outBuf;
					outBuf.resize(UNCOMPRESSED_SIZE);
					memcpy(&inBuf[0], Buffer, srcSize);

					/*decoding and decrypting the binary owo*/
					size_t dstLen = outBuf.size();
					size_t srcLen = inBuf.size() - LZMA_PROPS_SIZE;
					SRes res = LzmaUncompress(&outBuf[0], &dstLen, &inBuf[LZMA_PROPS_SIZE], &srcLen, &inBuf[0], LZMA_PROPS_SIZE);

					Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&outBuf[0], (uint32)dstLen));
					BlockDecrypted->Read(L"ProjectileInfoManager", Projectiles, false);

					delete[] Buffer;
				}

				ProjectileInfo* CompressedProjectileInfoManager::Retrieve(const uint32 hash) {
					if (compressedProjectiles.dataPosition.find(hash) == compressedProjectiles.dataPosition.end()) return nullptr;
					if (this->Projectiles.find(hash) != this->Projectiles.end()) return &Projectiles[hash];
					GetProjectile(compressedProjectiles.dataPosition[hash]);
					return &Projectiles[hash];
				}

				ProjectileInfo* CompressedProjectileInfoManager::Retrieve(const wchar_t* name) {
					return Retrieve(StringUtil::Hash(name));
				}

			}
		}
	}
}