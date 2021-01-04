#pragma once
#include "CompressedUpgradeTable.h"
#include <LzmaLib/LzmaLib.h>
#include <Core/FileIO/FileStream.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void CompressedUpgradeTableManager::Init(bool forceXml)
				{
					LoadBinaryData();
					LoadData();
				}

				void CompressedUpgradeTableManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/CompressedUpgradeTables.b");
					reader->Begin(L"AllM::XRated::Database::Info::UpgradeInfoManager");
					reader->Read(L"compressedUpgradeTables", compressedUpgradeTables);
					reader->Read(L"compressedNewUpgradeTables", compressedNewUpgradeTables);

					compressedUpgradeCbf = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/UpgradeTables.cbf");
				}

				void CompressedUpgradeTableManager::LoadData()
				{
					compressedUpgradeCbf->SetReadCursor(0, Lunia::IStream::Begin);
					/* Items */
					std::vector<uint8> Buffer;
					Buffer.resize(4);
					compressedUpgradeCbf->Read(&Buffer[0], 4);
					size_t BufferSize = (size_t)(*(int*)&Buffer[0]);

					Buffer.resize(BufferSize);
					compressedUpgradeCbf->Read(&Buffer[0], (uint32)BufferSize);
					IndexedUpgradeTablesCompressed = Buffer;

					/* Unidentified */
					Buffer.clear();
					Buffer.resize(4);
					compressedUpgradeCbf->Read(&Buffer[0], 4);
					size_t BufferUnidentifiedSize = (size_t)(*(int*)&Buffer[0]);

					Buffer.resize(BufferUnidentifiedSize);
					compressedUpgradeCbf->Read(&Buffer[0], (int)BufferUnidentifiedSize);
					IndexedNewUpgradeTablesCompressed = Buffer;

				}

				bool CompressedUpgradeTableManager::GetUpgradeTable(const uint32 templateOffset)
				{
					compressedUpgradeCbf = new FileIO::RefCountedMemoryStreamReader(IndexedUpgradeTablesCompressed.data(), (uint32)IndexedUpgradeTablesCompressed.size());
					compressedUpgradeCbf->SetReadCursor(templateOffset, Lunia::IStream::Begin);
					/* CompressedBlockSizeInBytes */
					uint8* Buffer = new uint8[4];

					/* Reading and setting a first block data in ReplayBuffer*/
					compressedUpgradeCbf->Read(Buffer, 4);
					size_t srcSize = (size_t)(*(int*)Buffer) + LZMA_PROPS_SIZE;
					compressedUpgradeCbf->Read(Buffer, 4);
					uint32 UNCOMPRESSED_SIZE = *(int*)Buffer;
					delete[] Buffer;

					Buffer = new uint8[srcSize];
					compressedUpgradeCbf->Read(Buffer, (int)srcSize);

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
					BlockDecrypted->Read(L"UpgradeTables", UpgradeTables, false);
					delete[] Buffer;
					return true;
				}

				bool CompressedUpgradeTableManager::GetNewUpgradeTable(const uint32 templateOffset)
				{
					compressedUpgradeCbf = new FileIO::RefCountedMemoryStreamReader(IndexedNewUpgradeTablesCompressed.data(), (uint32)IndexedNewUpgradeTablesCompressed.size());
					compressedUpgradeCbf->SetReadCursor(templateOffset, Lunia::IStream::Begin);
					/* CompressedBlockSizeInBytes */
					uint8* Buffer = new uint8[4];

					/* Reading and setting a first block data in ReplayBuffer*/
					compressedUpgradeCbf->Read(Buffer, 4);
					size_t srcSize = (size_t)(*(int*)Buffer) + LZMA_PROPS_SIZE;
					compressedUpgradeCbf->Read(Buffer, 4);
					uint32 UNCOMPRESSED_SIZE = *(int*)Buffer;
					delete[] Buffer;

					Buffer = new uint8[srcSize];
					compressedUpgradeCbf->Read(Buffer, (uint32)srcSize);

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
					BlockDecrypted->Read(L"NewUpgradeTables", NewUpgradeTables, false);
					delete[] Buffer;
					return true;
				}

				UpgradeTableInfo* CompressedUpgradeTableManager::Retrieve(const Database::Info::ItemInfo* potionItem, const Database::Info::ItemInfo* targetItem) {
					if (compressedUpgradeTables.dataPosition.find(potionItem->Id) == compressedUpgradeTables.dataPosition.end()) return nullptr;

					if (this->UpgradeTables.find(potionItem->Id) != this->UpgradeTables.end()) {
						if (this->UpgradeTables[potionItem->Id].find(targetItem->Hash) != this->UpgradeTables[potionItem->Id].end()) {
							return &UpgradeTables[potionItem->Id][targetItem->Hash];
						}
					}

					if (GetUpgradeTable(compressedUpgradeTables.dataPosition[potionItem->Id])) {
						return &UpgradeTables[potionItem->Id][targetItem->Hash];
					}
					else {
						Logger::GetInstance().Error(L"Unexpected Behavior - Item found on .b but not on .cbf ");
					}
					return nullptr;
				}

				const UpgradeTableInfo* CompressedUpgradeTableManager::RetrieveNewUpgrade(const Database::Info::ItemInfo* potionItem, const Database::Info::ItemInfo* targetItem) {
					if (compressedNewUpgradeTables.dataPosition.find(potionItem->Id) == compressedNewUpgradeTables.dataPosition.end()) return nullptr;

					auto newIt = this->NewUpgradeTables.find(potionItem->Id);
					if (newIt != NewUpgradeTables.end())
					{
						auto findIter = std::find_if(newIt->second.begin(), newIt->second.end(), UpgradeTableInfo::NewInfoFinder(targetItem->Limits.Class, targetItem->EquipParts));
						if (findIter != newIt->second.end())
						{
							return &(*findIter);
						}
					}
					else {
						if (GetNewUpgradeTable(compressedNewUpgradeTables.dataPosition[potionItem->Id])) {
							newIt = this->NewUpgradeTables.find(potionItem->Id);
							auto findIter = std::find_if(newIt->second.begin(), newIt->second.end(), UpgradeTableInfo::NewInfoFinder(targetItem->Limits.Class, targetItem->EquipParts));
							if (findIter != newIt->second.end()) {
								return &(*findIter);
							}
						}
						else {
							Logger::GetInstance().Error(L"Unexpected Behavior - Item found on .b but not on .cbf ");
						}
					}

					return nullptr;
				}
			}
		}
	}
}