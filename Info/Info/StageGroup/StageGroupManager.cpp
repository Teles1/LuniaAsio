#pragma once
#include "StageGroupManager.h"

namespace Lunia{
	namespace XRated {
		namespace Database {
			namespace Info {
				void StageGroupManager::Init(bool bForceXmlData)
				{
					if (bForceXmlData) //Xml loading
						LoadXmlData();
					else //Binary loading
						LoadBinaryData();
				}

				void StageGroupManager::LoadXmlData()
				{
					std::vector< uint32 > HighPriorityStageGroups, LowPriorityStageGroups;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StageGroups.xml");
					reader->Read(L"StageGroups", StageGroups);
					reader->Read(L"HighPriorityStageGroups", HighPriorityStageGroups);
					reader->Read(L"LowPriorityStageGroups", LowPriorityStageGroups);
					reader->Read(L"AfkCheckDisabledStageGroups", AfkCheckDisabledStageGroups);

					stageGroupPriority.clear();
					std::vector< uint32 >::const_iterator it = HighPriorityStageGroups.begin();
					for (auto count = HighPriorityStageGroups.size(); it != HighPriorityStageGroups.end(); ++it, --count)
					{
						stageGroupPriority[*it] = count;
					}

					it = LowPriorityStageGroups.begin();
					for (auto count = -1; it != LowPriorityStageGroups.end(); ++it, --count)
					{
						stageGroupPriority[*it] = count;
					}
				}

				void StageGroupManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StageGroups.b");
					reader->Read(L"StageGroupManager", *this);
				}

				StageGroup* StageGroupManager::Retrieve(uint32 hash)
				{
					StageGroupMap::iterator i = StageGroups.find(hash);
					if (i != StageGroups.end())
						return &(*i).second;
					return NULL;
				}

				StageGroup* StageGroupManager::Retrieve(const std::wstring& name)
				{
					return Retrieve(StringUtil::Hash(name.c_str()));
				}

				void StageGroupManager::SetStageInfo(StageInfoManager* stages)
				{
					StageGroupMap::iterator iInfo = StageGroups.begin();
					while (iInfo != StageGroups.end()) {
						(*iInfo).second.Stages.clear();
						for (uint32 i = 0; i < (*iInfo).second.StageHashes.size(); ++i)
							(*iInfo).second.Stages.push_back(stages->Retrieve((*iInfo).second.StageHashes[i]));
						++iInfo;
					}
				}

				void StageGroupManager::GetList(std::vector<std::wstring>& l)
				{
					StageGroupMap::iterator iInfo = StageGroups.begin();
					while (iInfo != StageGroups.end()) {
						l.push_back(iInfo->second.Name);
						++iInfo;
					}
				}

				void StageGroupManager::GetList(std::vector<uint32>& l)
				{
					StageGroupMap::iterator iInfo = StageGroups.begin();
					while (iInfo != StageGroups.end()) {
						l.push_back(iInfo->first);
						++iInfo;
					}
				}

				int StageGroupManager::GetStageGroupPriority(uint32 hash)
				{
					std::map< uint32, int >::const_iterator it = stageGroupPriority.find(hash);
					if (it != stageGroupPriority.end())
					{
						return it->second;
					}

					return 0;
				}

				bool StageGroupManager::IsAfkCheckEnabled(uint32 hash)
				{
					std::vector< uint32 >::const_iterator it = AfkCheckDisabledStageGroups.begin();
					for (; it != AfkCheckDisabledStageGroups.end(); ++it)
					{
						if (*it == hash)
						{
							return false;
						}
					}
					return true;
				}

				void StageGroupManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::StageGroupManager");
					out.Write(L"StageGroups", StageGroups);
					out.Write(L"StageGroupPriority", stageGroupPriority);
					out.Write(L"AfkCheckDisabledStageGroups", AfkCheckDisabledStageGroups);
				}

				void StageGroupManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::StageGroupManager");
					in.Read(L"StageGroups", StageGroups);
					in.Read(L"StageGroupPriority", stageGroupPriority);
					in.Read(L"AfkCheckDisabledStageGroups", AfkCheckDisabledStageGroups);
				}
			}
		}
	}
}