#pragma once
#include "StagePlayTimeManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void StagePlayTimeInfoManager::Init(bool bForceXmlData)
				{
					if (bForceXmlData) //Xml loading
						LoadXmlData();
					else //Binary loading
						LoadBinaryData();
				}

				void StagePlayTimeInfoManager::LoadXmlData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StagePlayTime.xml");
					reader->Read(L"StagePlayTimes", stagePlayTimes);
				}

				void StagePlayTimeInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StagePlayTime.b");
					reader->Read(L"StagePlayTimeInfoManager", *this);
				}
				StagePlayTime* StagePlayTimeInfoManager::Retrieve(uint32 stageHash)
				{
					StagePlayTimeContainer::iterator i = stagePlayTimes.find(stageHash);
					if (i != stagePlayTimes.end())
						return &(*i).second;
					return NULL;
				}
				void StagePlayTimeInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StagePlayTimeInfoManager");
					out.Write(L"StagePlayTimes", stagePlayTimes);
				}

				void StagePlayTimeInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StagePlayTimeInfoManager");
					in.Read(L"StagePlayTimes", stagePlayTimes);
				}

				void StagePlayTimeInfoManager::SetStagePlayTime(uint32 hash, StagePlayTime& data)
				{
					if (stagePlayTimes.count(hash) == 0)
					{
						stagePlayTimes[hash] = data;
					}
				}

			}
		}
	}
}
