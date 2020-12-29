#pragma once
#include "ActionsManager.h"
#include "../CompressedLoader.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class CompressedActionInfoManager : public XRated::Database::Info::ActionInfoManager
				{
				public:

					struct Actions : public Serializer::ISerializable
					{
						typedef std::map<std::wstring, uint32, Less<std::wstring> > CompressedActionMap;

						PositionData<CompressedActionMap> compressedActionMap;
						std::map<uint32, std::wstring> reverseMap;
						ActionInfoManager::Actions::ActorList actorList;


						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};


					std::map<std::wstring, Actions> compressedActionMap;
					std::map<uint16, std::vector<uint8>> IndexedActionsCompressed;
					Resource::StreamReader compressedActionsCbf;
					void Init();
					void LoadBinaryData();
					void GetData();
					void SaveXmlData();
					ActionInfoManager::Actions& Retrieve(const wchar_t* templateName);
					inline void ClearCache() { actionMap.clear(); }
				};
			}
		}
	}
}
