#include "PvPRankRewardItemTable.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				bool PvPRankRewardItemTable::GetRewardInfo(uint32 pvpRank, Info::PvPRankRewardInfo& rewardInfo)
				{
					if (infos.find(pvpRank) != infos.end())
					{
						rewardInfo = infos[pvpRank];
						return true;
					}

					return false;
				}

				void PvPRankRewardItemTable::Init(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/PvP/PvPRankRewardItem.xml");
						reader->Read(L"PvPRankRewardItemTable", infos);
					}
					else
					{
						Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/PvPRankRewardItem.b");
						reader->Read(L"PvPRankRewardItemTable", infos);
					}
				}

				void PvPRankRewardItemTable::Save(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamWriter writer = Lunia::Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/PvP/PvPRankRewardItem.xml");
						writer->Write(L"PvPRankRewardItemTable", infos);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/PvPRankRewardItem.b");
						writer->Write(L"PvPRankRewardItemTable", infos);
					}
				}
			}
		}
	}
}