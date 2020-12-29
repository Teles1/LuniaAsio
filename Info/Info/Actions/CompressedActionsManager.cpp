#pragma once
#include "CompressedActionsManager.h"
#include <iostream>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				std::vector<std::wstring> asodjasidjasid;
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

					asodjasidjasid.push_back(std::prev(actorList.end())->first);
				}

				void CompressedActionInfoManager::SaveXmlData() {
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"CompressedActionsInfos.xml");
					writer->Begin(L"AllM::XRated::Database::Info::CompressedActionInfoManager::Actions");
					writer->Write(L"compressedActionMap", compressedActionMap);
				}

				void CompressedActionInfoManager::Init()
				{
					LoadBinaryData();
				}

				void CompressedActionInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/CompressedActionInfos.b");
					reader->Read(L"compressedActionMap", compressedActionMap);

					compressedActionsCbf = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/ActionInfos.cbf");
				}

				void CompressedActionInfoManager::GetData()
				{
					uint16 index = 0;
					compressedActionsCbf->SetReadCursor(0, Lunia::IStream::Begin);
					for (int i = 0; i < 756; i++) {
						uint8* SizeNpc = reinterpret_cast<uint8*>(new char[4]);
						compressedActionsCbf->Read(SizeNpc, 4);
						std::cout << std::hex << compressedActionsCbf->GetReadCursor() << std::endl;
						std::wcout << asodjasidjasid[i] << std::endl;
						size_t srcSize = *(int*)SizeNpc;
						std::vector<uint8> lReplayBuffer;
						lReplayBuffer.resize(srcSize);
						compressedActionsCbf->Read(&lReplayBuffer[0], srcSize);
						IndexedActionsCompressed[index] = lReplayBuffer;
						index++;
					}
					std::cout << std::hex << compressedActionsCbf->GetReadCursor() << std::endl;
					compressedActionsCbf->SetReadCursor(compressedActionsCbf->GetReadCursor() + 4, Lunia::IStream::Begin);
					for (int i = 0; i < 612; i++) {
						uint8* SizeNpc = reinterpret_cast<uint8*>(new char[4]);
						compressedActionsCbf->Read(SizeNpc, 4);
						//std::cout << std::hex << compressedActionsCbf->GetReadCursor() << std::endl;
						//std::wcout << asodjasidjasid[i+757] << std::endl;
						size_t srcSize = *(int*)SizeNpc;
						std::vector<uint8> lReplayBuffer;
						lReplayBuffer.resize(srcSize);
						compressedActionsCbf->Read(&lReplayBuffer[0], srcSize);
						IndexedActionsCompressed[index] = lReplayBuffer;
						index++;
					}
					//std::cout << std::hex << compressedActionsCbf->GetReadCursor() << std::endl;
				}

				ActionInfoManager::Actions& CompressedActionInfoManager::Retrieve(const wchar_t* templateName)
				{
					return actionMap[templateName];
				}
			}
		}
	}
}
