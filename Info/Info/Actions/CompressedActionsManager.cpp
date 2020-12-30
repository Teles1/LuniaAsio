#pragma once
#include "CompressedActionsManager.h"
#include <iostream>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				std::vector<std::wstring> actionNames;
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

					actionNames.push_back( std::prev( actorList.end() )->first );
				}

				void CompressedActionInfoManager::SaveXmlData() {
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"CompressedActionsInfos.xml");
					writer->Begin(L"AllM::XRated::Database::Info::CompressedActionInfoManager::Actions");
					writer->Write(L"compressedActionMap", compressedActionMap);
				}

				void CompressedActionInfoManager::Init()
				{
					LoadBinaryData();
					LoadCBFInData();
				}

				void CompressedActionInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/CompressedActionInfos.b");
					reader->Read(L"compressedActionMap", compressedActionMap);

					compressedActionsCbf = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/ActionInfos.cbf");
				}

				void CompressedActionInfoManager::LoadCBFInData()
				{
					compressedActionsCbf->SetReadCursor(0, Lunia::IStream::Begin);
					for (int i = 0; i < actionNames.size(); i++) {
						if (actionNames[i] == L"") {
							compressedActionsCbf->SetReadCursor(compressedActionsCbf->GetReadCursor() + 4, Lunia::IStream::Begin); 
							continue;
						}
						uint8* SizeNpc = reinterpret_cast<uint8*>(new char[4]);
						compressedActionsCbf->Read(SizeNpc, 4);
						uint32 srcSize = *(int*)SizeNpc;
						std::vector<uint8> lReplayBuffer;
						lReplayBuffer.resize(srcSize);
						compressedActionsCbf->Read(&lReplayBuffer[0], (uint32)srcSize);
						IndexedActionsCompressed[i] = lReplayBuffer;
					}
				}

				void CompressedActionInfoManager::GetData(const uint32 index)
				{
					compressedActionsCbf = new FileIO::RefCountedMemoryStreamReader(&IndexedActionsCompressed[index][0], (uint32)IndexedActionsCompressed[index].size());

					std::vector<uint8> Buffer;
					Buffer.resize(4);
					size_t COMPRESSED_SIZE = 0;
					size_t UNCOMPRESSED_SIZE = 0;

					while (compressedActionsCbf->GetSizeLeft() > 0) {
						compressedActionsCbf->Read(&Buffer[0], 4);
						COMPRESSED_SIZE = *(int*)&Buffer[0];
						compressedActionsCbf->Read(&Buffer[0], 4);
						UNCOMPRESSED_SIZE = *(int*)&Buffer[0];

						/* Setting buffer input and output sizes*/
						std::vector<uint8> inBuf;
						inBuf.resize(COMPRESSED_SIZE + LZMA_PROPS_SIZE);
						std::vector<uint8> outBuf;
						outBuf.resize(UNCOMPRESSED_SIZE);

						compressedActionsCbf->Read(inBuf.data(), (uint32)inBuf.size());

						/*decoding and decrypting the binary owo*/
						SRes res = LzmaUncompress(outBuf.data(), &UNCOMPRESSED_SIZE, inBuf.data() + LZMA_PROPS_SIZE, &COMPRESSED_SIZE, inBuf.data(), LZMA_PROPS_SIZE);

						Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(
							new FileIO::RefCountedMemoryStreamReader(&outBuf[0], (uint32)UNCOMPRESSED_SIZE)
						);
						BlockDecrypted->Read(L"ActionsInfoManager", actions, false);
					}
				}

				ActionInfo* CompressedActionInfoManager::Retrieve(const wchar_t* actionName)
				{
					for (int i = 0; i < actionNames.size(); i++) {
						if (actionNames[i] == actionName) {
							GetData(i);
							return &actions[actionName];
						}
					}
					return nullptr;
				}
			}
		}
	}
}
