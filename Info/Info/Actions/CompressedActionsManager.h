#pragma once
#include "ActionsManager.h"
#include "../CompressedLoader.h"
#include <Core/FileIO/FileStream.h>
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
					virtual void LoadBinaryData();
					ActionInfoManager::Actions& Retrieve(const int templateOffset);
					inline void ClearCache() { actionMap.clear(); }
				};
			}
		}
	}
}