#pragma once
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


					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"CompressedItemInfos.xml");
					writer->Begin(L"AllM::XRated::Database::Info::CompressedActionInfoManager::Actions");
					writer->Write(L"compressedActionMap", compressedActionMap);
					writer->Write(L"reverseMap", reverseMap);
					writer->Write(L"actorList", actorList);
				}

				void CompressedActionInfoManager::LoadBinaryData()
				{

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/CompressedActionInfos.b");
					std::map<std::wstring, Actions> compressedActionMap;
					reader->Read(L"compressedActionMap", compressedActionMap);
				}

				ActionInfoManager::Actions& CompressedActionInfoManager::Retrieve(const wchar_t* templateName)
				{
					return actionMap[templateName];
				}
			}
		}
	}
}