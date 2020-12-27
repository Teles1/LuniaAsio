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

				ActionInfoManager::Actions& CompressedActionInfoManager::Retrieve(const wchar_t* templateName)
				{
					return actionMap[templateName];
				}
			}
		}
	}
}