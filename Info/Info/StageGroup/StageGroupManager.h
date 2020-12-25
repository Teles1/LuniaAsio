#pragma once
#include "StageGroup.h"
#include <Info/Info/StageInfo/StageInfoManager.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class StageGroupManager : public Serializer::ISerializable {
				public:
					typedef std::map<uint32, StageGroup, Less<uint32> > StageGroupMap;

				protected:
					StageGroupMap StageGroups;
					std::map< uint32, int > stageGroupPriority;
					std::vector< uint32 > AfkCheckDisabledStageGroups;
				public:
					void Init(bool bForceXml);
					void LoadXmlData();
					void SaveXml();
					virtual void LoadBinaryData();

					void SetStageInfo(StageInfoManager* stages);

					virtual StageGroup* Retrieve(uint32 hash);
					virtual StageGroup* Retrieve(const std::wstring& name);
					void GetList(std::vector<std::wstring>& l);
					void GetList(std::vector<uint32>& l);

					// by Caspian
					typedef StageGroupMap::iterator StageGroupIter;
					inline StageGroupIter Begin() { return StageGroups.begin(); }
					inline StageGroupIter End() { return StageGroups.end(); }
					inline size_t Size() { return StageGroups.size(); }

					int GetStageGroupPriority(uint32 hash);
					bool IsAfkCheckEnabled(uint32 hash);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}