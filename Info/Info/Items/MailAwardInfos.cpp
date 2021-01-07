#include "MailAwardInfos.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				const AwardInfo* MailAwardItemTable::GetLevelUpAwardInfo(int level) const
				{
					std::map< int, AwardInfo >::const_iterator iter = levelupAwardInfos.find(level);
					if (iter != levelupAwardInfos.end())
					{
						return &iter->second;
					}
					return NULL;
				}

				const AwardInfo* MailAwardItemTable::GetLicenseAwardInfo(uint32 stageGroupHash, int level) const
				{
					std::map< std::pair< uint32, int>, AwardInfo >::const_iterator iter = licenseAwardInfos.find(std::make_pair(stageGroupHash, level));
					if (iter != licenseAwardInfos.end())
					{
						return &iter->second;
					}
					return NULL;
				}

				const AwardInfo* MailAwardItemTable::GetGuildContributedAwardInfo(int contributed) const
				{
					if (guildContributedAwardInfos.empty()) return NULL;

					std::map< int, AwardInfo >::const_iterator selectIter = guildContributedAwardInfos.end();
					int contributedVal = 0;

					std::map< int, AwardInfo >::const_iterator iter = guildContributedAwardInfos.begin();
					while (iter != guildContributedAwardInfos.end())
					{
						if (((contributed % iter->first) == 0) && (contributedVal < iter->first))
						{
							selectIter = iter;
							contributedVal = iter->first;
						}

						++iter;
					}

					if (selectIter != guildContributedAwardInfos.end())
					{
						return &selectIter->second;
					}
					return NULL;
				}

				bool MailAwardItemTable::IsGetGuildContributedAward(int contributed)
				{
					if (guildContributedAwardInfos.empty()) return false;

					std::map< int, AwardInfo >::const_iterator iter = guildContributedAwardInfos.begin();
					while (iter != guildContributedAwardInfos.end())
					{
						if ((contributed % iter->first) == 0) return true;
						++iter;
					}

					return false;
				}


				void MailAwardItemTable::Init(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Items/Mail/MailRewardSystem.xml");
						reader->Read(L"LevelUpAwardItemTable", levelupAwardInfos);
						reader->Read(L"LicenseAwardInfos", licenseAwardInfos);
						reader->Read(L"GuildContributedAwardItemTable", guildContributedAwardInfos);
					}
					else
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/MailRewardSystem.b");
						reader->Read(L"LevelUpAwardItemTable", levelupAwardInfos);
						reader->Read(L"LicenseAwardInfos", licenseAwardInfos);
						reader->Read(L"GuildContributedAwardItemTable", guildContributedAwardInfos);
					}
				}
				void MailAwardItemTable::Save(bool xml)
				{
					if (xml == true)
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Items/Mail/MailRewardSystem.xml");
						writer->Write(L"LevelUpAwardItemTable", levelupAwardInfos);
						writer->Write(L"LicenseAwardInfos", licenseAwardInfos);
						writer->Write(L"GuildContributedAwardItemTable", guildContributedAwardInfos);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/MailRewardSystem.b");
						writer->Write(L"LevelUpAwardItemTable", levelupAwardInfos);
						writer->Write(L"LicenseAwardInfos", licenseAwardInfos);
						writer->Write(L"GuildContributedAwardItemTable", guildContributedAwardInfos);
					}
				}

			}
		}
	}
}