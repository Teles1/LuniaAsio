#pragma once
#include "CompressedActionsManager.h"
#include <Core/FileIO/FileStream.h>
#include <LzmaLib/LzmaLib.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void CompressedActionInfoManager::SaveXmlData() {
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"CompressedActionsInfos.xml");
					writer->Begin(L"AllM::XRated::Database::Info::CompressedActionInfoManager::Actions");
					writer->Write(L"compressedActionMap", actionMap);
				}

				void CompressedActionInfoManager::Init(bool forceXml)
				{
					LoadBinaryData();
					LoadCBFInData();
				}

				void CompressedActionInfoManager::LoadBinaryData()
				{
					Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/CompressedActionInfos.b")
						->Read(L"compressedActionMap", actionMap);
				}

				void CompressedActionInfoManager::LoadCBFInData()
				{
					Resource::StreamReader compressedActionsCbf = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/ActionInfos.cbf");
					compressedActionsCbf->SetReadCursor(0, Lunia::IStream::Begin);
					for (auto& itr : actionMap) {
						if (itr.second.compressedActionMap.size() == 0) {
							compressedActionsCbf->SetReadCursor(compressedActionsCbf->GetReadCursor() + 4, Lunia::IStream::Begin); 
							itr.second.compressedData = { 0x0,0x0,0x0,0x0 }; //prevent the missmatch in the size (annoys me plus it's only 4 bytes. come on!)
							// this might not be needed but i'm doing it anyways.
							continue;
						}
						uint8* buffer = new uint8[4];
						compressedActionsCbf->Read(buffer, 4);
						uint32 srcSize = (size_t)(*(int*)buffer);
						std::vector<uint8> lReplayBuffer;
						lReplayBuffer.resize(srcSize);
						compressedActionsCbf->Read(&lReplayBuffer[0], (uint32)srcSize);
						itr.second.compressedData = std::move(lReplayBuffer);
					}
				}
				void CompressedActionInfoManager::GetData(ActionInfoManager::Actions& actionMap)
				{
					Resource::StreamReader reader = new FileIO::RefCountedMemoryStreamReader( &actionMap.compressedData[0], uint32(actionMap.compressedData.size()) );

					std::vector<uint8> Buffer;
					Buffer.resize(4);
					size_t COMPRESSED_SIZE = 0;
					size_t UNCOMPRESSED_SIZE = 0;

					while (reader->GetSizeLeft() > 0) {
						reader->Read(&Buffer[0], 4);
						COMPRESSED_SIZE = *(int*)&Buffer[0];
						
						reader->Read(&Buffer[0], 4);
						UNCOMPRESSED_SIZE = *(int*)&Buffer[0];

						/* Setting buffer input and output sizes*/
						std::vector<uint8> inBuf;
						inBuf.resize(COMPRESSED_SIZE + LZMA_PROPS_SIZE);
						std::vector<uint8> outBuf;
						outBuf.resize(UNCOMPRESSED_SIZE);

						reader->Read(inBuf.data(), (uint32)inBuf.size());

						/*decoding and decrypting the binary owo*/
						SRes res = LzmaUncompress(outBuf.data(), &UNCOMPRESSED_SIZE, inBuf.data() + LZMA_PROPS_SIZE, &COMPRESSED_SIZE, inBuf.data(), LZMA_PROPS_SIZE);

						Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(
							new FileIO::RefCountedMemoryStreamReader(&outBuf[0], (uint32)UNCOMPRESSED_SIZE)
						);
						BlockDecrypted->Read(L"ActionsInfoManager", actionMap.actions, false);
					}
					actionMap.compressedData.clear();
				}
				/*
				ActionInfo* CompressedActionInfoManager::Retrieve(const wchar_t* actionName)
				{
					for (int i = 0; i < actionNames.size(); i++) {
						if (actionNames[i] == actionName) {
							GetData(i);
							return &actions[actionName];
						}
					}
					return nullptr;
				}*/
				ActionInfoManager::Actions& CompressedActionInfoManager::Retrieve(const wchar_t* templateName) {
					auto i = actionMap.find(templateName);
					
					if (i != actionMap.end())
					{
						//if there is actions in the list but the number of actionsInfo loaded doesn't match it means that we should be reading the cbf.
						//if not then we just return.
						if((*i).second.actions.size() != (*i).second.compressedActionMap.size()) 
							GetData((*i).second); // this clears the compresseddata buffer btw.
						return (*i).second;
					}
					LoggerInstance().Exception(L"Unable to find action by template name({0})", templateName);
					throw;
				}
			}
		}
	}
}
