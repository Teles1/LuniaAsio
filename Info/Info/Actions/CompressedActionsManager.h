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

					virtual void LoadBinaryData();
					void SaveXmlData();
					ActionInfoManager::Actions& Retrieve(const wchar_t* templateName);
					inline void ClearCache() { actionMap.clear(); }
				};
			}
		}
	}
}