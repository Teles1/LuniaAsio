#pragma once
#include "PvPRewardItemInfosManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				bool PvPRewardItemTable::GetRewardInfo(uint32 stageHash, Info::PvPRewardInfo& rewardInfo)
				{
					if (infos.find(stageHash) != infos.end())
					{
						rewardInfo = infos[stageHash];
						return true;
					}

					return false;
				}
				void PvPRewardItemTable::Load(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/PvP/PvPRewardItem.xml");
						reader->Read(L"PvPRewardItemTable", infos);
					}
					else
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/PvPRewardItem.b");
						reader->Read(L"PvPRewardItemTable", infos);
					}
				}
				void PvPRewardItemTable::Save(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/PvP/PvPRewardItem.xml");
						writer->Write(L"PvPRewardItemTable", infos);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/PvPRewardItem.b");
						writer->Write(L"PvPRewardItemTable", infos);
					}
				}
			}
		}
	}
}
