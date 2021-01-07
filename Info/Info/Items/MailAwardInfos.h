#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/Items/AwardInfo.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				class MailAwardItemTable
				{
				public:
					const AwardInfo* GetLevelUpAwardInfo(int level) const;
					const AwardInfo* GetLicenseAwardInfo(uint32 stageGroupHash, int level) const;
					const AwardInfo* GetGuildContributedAwardInfo(int contributed) const;

					bool IsGetGuildContributedAward(int contributed);

					void Init(bool xml);
					void Save(bool xml);
				private:
					std::map< int /*level*/, AwardInfo > levelupAwardInfos;
					std::map< std::pair<uint32  /*stageGroupHash*/, int /*level*/>, AwardInfo > licenseAwardInfos;
					std::map< int /*Contibuted*/, AwardInfo > guildContributedAwardInfos;
				};
			}
		}
	}
}