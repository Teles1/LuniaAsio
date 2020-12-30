#pragma once
#include "StageInfo.h"
#include <Info/Info/ItemStageInfo/ItemStageInfo.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class StageInfoManager : public Serializer::ISerializable
				{
				public:
					typedef std::map<uint32, StageInfo, Less<uint32> > StageInfoMap;
					typedef std::map<uint32, StageInfo::MoveMap> MoveMapMap;
					struct ItemStageInfoContainer : public Serializer::ISerializable {
						typedef std::vector<ItemStageInfo> ItemStageInfoVector;
						typedef std::vector<ItemStageInfo::DropTable> DropTableVector;
						typedef std::vector<ItemStageInfo::DropProbability> DropProbabilityVector;

					public:
						DropTableVector DropTables;
						DropProbabilityVector DropProbabilities;
						ItemStageInfoVector ItemStagesInfos;

						std::vector<ItemStageInfo::ItemStageInfoData> ItemStageInfoDataList; ///< After loading file data, Make stage info combining data for fast search.
					public:
						template < typename Iter> typename Iter::value_type* FindValue(uint32 hash, Iter begin, Iter end) {
							while (begin != end) {
								if ((*begin).Hash == hash) return &(*begin);
								++begin;
							}
							return NULL;
						}
						ItemStageInfo::ItemStageInfoData* Retrieve(uint32 hash) { return FindValue(hash, ItemStageInfoDataList.begin(), ItemStageInfoDataList.end()); }
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

				protected:
					StageInfoMap stages;
					ItemStageInfoContainer itemStageInfoContainer;

					MoveMapMap moveMaps;

					void LoadStage(const wchar_t* id, bool bPreloadMovemap, bool bPreloadScript);
					void LoadPvpItemStageInfo();

				public:
					//#ifdef ALLM_COMPILE_CEGUI
					StageInfoMap::iterator StageMapBegin() { return stages.begin(); }
					StageInfoMap::iterator StageMapEnd() { return stages.end(); }
					//#endif
				public:
					StageInfoManager();
					~StageInfoManager();

					void Init(bool bForceXml);
					void LoadXmlData();
					void SaveXml();
					void LoadBinaryData();

					void GetStageList(std::vector<std::wstring>& l);
					StageInfo* Retrieve(uint32 hash);
					StageInfo* Retrieve(const wchar_t* id);

					ItemStageInfo::ItemStageInfoData* RetrieveItemStageInfo(uint32 hash);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
