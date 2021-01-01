#ifndef FamilyInfoManager_H
#define FamilyInfoManager_H
#pragma once
#ifndef FamilyInfo_H
#include <Info/Info/GuildFamily/FamilyInfo.h>
#endif // !FamilyInfo_H
#ifndef DateTime_H
#include<Core/Utils/DateTime.h>
#endif // !DateTime_H
#ifndef StageClearPresentInfo_H
#include <Info/Info/ETC/StageClearPresentInfo.h>
#endif // !StageClearPresentInfo_H

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class FamilyInfoManager
				{
				public:
					const FamilyBasicInfo& GetBasicInfo() const;
					const FamilyPassvieRewardInfo* GetPassiveReward(uint16 regularMemberCount, uint16 guestMemberCount) const;
					const FamilyPresentConditionInfo& GetPresentConditionInfo() const;
				public:
					bool IsRewardWithRegularMemberFromLevel(uint16 level) const;
					bool IsRewardWithGuestMemberFromLevel(uint16 level) const;
					bool IsRewardFromFishingItem(uint32 hash) const;

					const FamilyPresentInfo& GetRebirthPresent() const;
					const FamilyPresentInfo& GetFishingPresent() const;
					const FamilyPresentInfo& GetGroupPlayPresent() const;
					const FamilyPresentInfo* GetLevelUpPresent(uint16 level, bool isGuest) const;
					const FamilyPresentInfo* GetStoredLevelUpPresent(uint32 storedLevel, bool isGuest) const;
					const FamilyPresentInfo* GetPersonalPlayPresent(int64 playTime) const;
					int64 GetNextPersonalPlayPresent(int64 playTime) const;
					uint16 GetNextLevelFromLevelPresent(uint16 level, bool isGuest) const;
					uint32 GetNextStoredLevelFromStoredLevelPresent(uint32 level, bool isGuest) const;
					const FamilyPresentInfo* GetMemorialDayPresent(uint32 day) const;
					DateTime GetNextMemorialDayPresentDate(DateTime created, DateTime now) const;

					const FamilyPresentInfo* GetStageClearPresent(uint32 stageInfoHash, uint16 onlineRegularMember, uint16 onlineGuestMember) const;
					const StageClearPresentInfo* GetStateClearPresentInfo(uint32 stageInfoHash) const;
				public:
					void Init(bool xml);
					void Save(bool xml);

					FamilyInfoManager();
					virtual ~FamilyInfoManager();
				private:
					void LoadPresentInfoForXml(const std::wstring& file, FamilyPresentInfo& info);
					void SavePresentInfoForXml(const std::wstring& file, const FamilyPresentInfo& info);
				private:
					FamilyBasicInfo	basicInfo;
					FamilyPresentConditionInfo	presentCondition;
					//
					typedef std::vector<FamilyPassvieRewardInfo> FamilyPassvieRewardInfoList;
					FamilyPassvieRewardInfoList	familyPassvieRewardInfos;


					FamilyPresentInfo		rebirthPresent;	
					FamilyPresentInfo		fishingPresent;	
					FamilyPresentInfo		groupPlayPresent;


					struct PersonalPlayTimePresentInfo
					{
						uint32				DivideUnit;
						FamilyPresentInfo	Present;
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					std::vector<PersonalPlayTimePresentInfo>		personalPlayPresents;

					std::map< uint16, FamilyPresentInfo>			regularMemberLevelTable;
					std::map< uint16, FamilyPresentInfo>			guestMemberLevelTable;

					std::map< uint32, FamilyPresentInfo>			regularMemberStoredLevelTable;
					std::map< uint32, FamilyPresentInfo>			guestMemberStoredLevelTable;


					std::vector<MemorialDayPresentInfo>			memorialDayPresents;



					std::map< uint32/*stage info hash*/, StageClearPresentInfo>		stageClearPresentTable;
				};
			}
		}
	}
}
#endif // !FamilyInfoManager_H