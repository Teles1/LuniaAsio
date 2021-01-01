#include "FamilyInfoManager.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void FamilyInfoManager::PersonalPlayTimePresentInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::PersonalPlayTimePresentInfo");
					out.Write(L"DivideUnit", DivideUnit);
					out.Write(L"Present", Present);
				}
				void FamilyInfoManager::PersonalPlayTimePresentInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::PersonalPlayTimePresentInfo");
					in.Read(L"DivideUnit", DivideUnit);
					in.Read(L"Present", Present);
				}

				FamilyInfoManager::FamilyInfoManager() {}
				FamilyInfoManager::~FamilyInfoManager() {}
				void FamilyInfoManager::Init(bool xml)
				{
					if (xml == true)
					{
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/FamilyInfo.xml");
							reader->Read(L"BasicInfo", basicInfo);
							reader->Read(L"PresentCondition", presentCondition);
							reader->Read(L"FamilyPassvieRewardInfoList", familyPassvieRewardInfos);
						}
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/PersonalPlayPresentInfo.xml");
							reader->Read(L"PersonalPlayPresentList", personalPlayPresents);
						}
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/RegularMemberLevelPresentTable.xml");
							reader->Read(L"RegularMemberLevelTable", regularMemberLevelTable);
						}
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/GuestMemberLevelPresentTable.xml");
							reader->Read(L"GuestMemberLevelTable", guestMemberLevelTable);
						}

						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/RegularMemberStoredLevelPresentTable.xml");
							reader->Read(L"RegularMemberStoredLevelTable", regularMemberStoredLevelTable);
						}
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/GuestMemberStoredLevelPresentTable.xml");
							reader->Read(L"GuestMemberStoredLevelTable", guestMemberStoredLevelTable);
						}
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/MemorialDayPresentTable.xml");
							reader->Read(L"MemorialDayPresentTable", memorialDayPresents);
						}
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Family/StageClearPresentTable.xml");
							reader->Read(L"StageClearPresentTable", stageClearPresentTable);
						}
						LoadPresentInfoForXml(L"Database/Family/RebirthPresentInfo.xml", rebirthPresent);
						LoadPresentInfoForXml(L"Database/Family/FishingPresentInfo.xml", fishingPresent);
						LoadPresentInfoForXml(L"Database/Family/GroupPlayPresentInfo.xml", groupPlayPresent);
					}
					else
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/FamilyInfo.b");
						reader->Read(L"BasicInfo", basicInfo);
						reader->Read(L"PresentCondition", presentCondition);
						reader->Read(L"FamilyPassvieRewardInfoList", familyPassvieRewardInfos);

						reader->Read(L"RegularMemberLevelTable", regularMemberLevelTable);
						reader->Read(L"GuestMemberLevelTable", guestMemberLevelTable);
						reader->Read(L"RegularMemberStoredLevelTable", regularMemberStoredLevelTable);
						reader->Read(L"GuestMemberStoredLevelTable", guestMemberStoredLevelTable);
						reader->Read(L"RebirthPresent", rebirthPresent);
						reader->Read(L"FishingPresent", fishingPresent);
						reader->Read(L"GroupPlayPresent", groupPlayPresent);
						reader->Read(L"PersonalPlayPresentList", personalPlayPresents);
						reader->Read(L"MemorialDayPresentTable", memorialDayPresents);
						reader->Read(L"StageClearPresentTable", stageClearPresentTable);
					}
				}
				void FamilyInfoManager::LoadPresentInfoForXml(const std::wstring& file, FamilyPresentInfo& info)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(file.c_str());
					reader->Read(L"PresentInfo", info);
				}

				void FamilyInfoManager::Save(bool xml)
				{
					if (xml == true)
					{
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/FamilyInfo.xml");
							writer->Write(L"BasicInfo", basicInfo);
							writer->Write(L"PresentCondition", presentCondition);
							writer->Write(L"FamilyPassvieRewardInfoList", familyPassvieRewardInfos);
						}
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/PersonalPlayPresentInfo.xml");
							writer->Write(L"PersonalPlayPresentList", personalPlayPresents);
						}
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/RegularMemberLevelPresentTable.xml");
							writer->Write(L"RegularMemberLevelTable", regularMemberLevelTable);
						}
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/GuestMemberLevelPresentTable.xml");
							writer->Write(L"GuestMemberLevelTable", guestMemberLevelTable);
						}
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/RegularMemberStoredLevelPresentTable.xml");
							writer->Write(L"RegularMemberStoredLevelTable", regularMemberStoredLevelTable);
						}
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/GuestMemberStoredLevelPresentTable.xml");
							writer->Write(L"GuestMemberStoredLevelTable", guestMemberStoredLevelTable);
						}
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/MemorialDayPresentTable.xml");
							writer->Write(L"MemorialDayPresentTable", memorialDayPresents);
						}
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Family/StageClearPresentTable.xml");
							writer->Write(L"StageClearPresentTable", stageClearPresentTable);
						}
						SavePresentInfoForXml(L"Database/Family/RebirthPresentInfo.xml", rebirthPresent);
						SavePresentInfoForXml(L"Database/Family/FishingPresentInfo.xml", fishingPresent);
						SavePresentInfoForXml(L"Database/Family/GroupPlayPresentInfo.xml", groupPlayPresent);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/FamilyInfo.b");
						writer->Write(L"BasicInfo", basicInfo);
						writer->Write(L"PresentCondition", presentCondition);
						writer->Write(L"FamilyPassvieRewardInfoList", familyPassvieRewardInfos);

						writer->Write(L"RegularMemberLevelTable", regularMemberLevelTable);
						writer->Write(L"GuestMemberLevelTable", guestMemberLevelTable);
						writer->Write(L"RegularMemberStoredLevelTable", regularMemberStoredLevelTable);
						writer->Write(L"GuestMemberStoredLevelTable", guestMemberStoredLevelTable);
						writer->Write(L"RebirthPresent", rebirthPresent);
						writer->Write(L"FishingPresent", fishingPresent);
						writer->Write(L"GroupPlayPresent", groupPlayPresent);
						writer->Write(L"PersonalPlayPresentList", personalPlayPresents);
						writer->Write(L"MemorialDayPresentTable", memorialDayPresents);
						writer->Write(L"StageClearPresentTable", stageClearPresentTable);
					}
				}
				void FamilyInfoManager::SavePresentInfoForXml(const std::wstring& file, const FamilyPresentInfo& info)
				{
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(file.c_str());
					writer->Write(L"PresentInfo", info);
				}

				const FamilyBasicInfo& FamilyInfoManager::GetBasicInfo() const
				{
					return basicInfo;
				}
				const FamilyPassvieRewardInfo* FamilyInfoManager::GetPassiveReward(uint16 regularMemberCount, uint16 guestMemberCount) const
				{
					size_t max = familyPassvieRewardInfos.size();
					for (size_t i = 0; i < max; ++i)
					{
						const FamilyPassvieRewardInfo& info = familyPassvieRewardInfos[i];
						if ((info.EnableToOnlineRegularMemberCount == regularMemberCount)
							&& (info.EnableToOnlineGeustMemberCount == guestMemberCount))
						{
							return &(familyPassvieRewardInfos[i]);
						}
					}
					return NULL;
				}
				const FamilyPresentConditionInfo& FamilyInfoManager::GetPresentConditionInfo() const
				{
					return presentCondition;
				}
				bool FamilyInfoManager::IsRewardWithRegularMemberFromLevel(uint16 level) const
				{
					std::map< uint16, FamilyPresentInfo>::const_iterator iter = regularMemberLevelTable.find(level);
					if (iter != regularMemberLevelTable.end()) {
						return true;
					}
					return false;
				}
				bool FamilyInfoManager::IsRewardWithGuestMemberFromLevel(uint16 level) const
				{
					std::map< uint16, FamilyPresentInfo>::const_iterator iter = guestMemberLevelTable.find(level);
					if (iter != guestMemberLevelTable.end()) {
						return true;
					}
					return false;
				}
				bool FamilyInfoManager::IsRewardFromFishingItem(uint32 hash) const
				{
					return presentCondition.FishingItemHash == hash;
				}
				const FamilyPresentInfo* FamilyInfoManager::GetLevelUpPresent(uint16 level, bool isGuest) const
				{
					if (isGuest == true) {
						std::map< uint16, FamilyPresentInfo>::const_iterator iter = guestMemberLevelTable.find(level);
						if (iter != guestMemberLevelTable.end()) {
							return &(iter->second);
						}
					}
					else {
						std::map< uint16, FamilyPresentInfo>::const_iterator iter = regularMemberLevelTable.find(level);
						if (iter != regularMemberLevelTable.end()) {
							return &(iter->second);
						}
					}
					return NULL;
				}
				const FamilyPresentInfo* FamilyInfoManager::GetStoredLevelUpPresent(uint32 storedLevel, bool isGuest) const
				{
					if (isGuest == true) {
						std::map< uint32, FamilyPresentInfo>::const_iterator iter = guestMemberStoredLevelTable.find(storedLevel);
						if (iter != guestMemberStoredLevelTable.end()) {
							return &(iter->second);
						}
					}
					else {
						std::map< uint32, FamilyPresentInfo>::const_iterator iter = regularMemberStoredLevelTable.find(storedLevel);
						if (iter != regularMemberStoredLevelTable.end()) {
							return &(iter->second);
						}
					}
					return NULL;
				}
				const FamilyPresentInfo& FamilyInfoManager::GetRebirthPresent() const
				{
					return rebirthPresent;
				}
				const FamilyPresentInfo& FamilyInfoManager::GetFishingPresent() const
				{
					return fishingPresent;
				}
				const FamilyPresentInfo& FamilyInfoManager::GetGroupPlayPresent() const
				{
					return groupPlayPresent;
				}
				const FamilyPresentInfo* FamilyInfoManager::GetPersonalPlayPresent(int64 playTime) const
				{
					int64 unit = playTime / static_cast<int64>(presentCondition.PersonalPlayTimeUnit * 60);
					size_t max = personalPlayPresents.size();
					for (size_t i = 0; i < max; ++i)
					{
						if ((unit != 0) && ((unit / static_cast<int64>(personalPlayPresents[i].DivideUnit)) != 0)
							&& ((unit % static_cast<int64>(personalPlayPresents[i].DivideUnit)) == 0))
						{
							return &(personalPlayPresents[i].Present);
						}
					}
					return NULL;
				}
				int64 FamilyInfoManager::GetNextPersonalPlayPresent(int64 playTime) const
				{
					int64 unit = static_cast<int64>(presentCondition.PersonalPlayTimeUnit * 60);
					return (static_cast<int64>((playTime + unit) / (unit)) * unit);

				}
				uint16 FamilyInfoManager::GetNextLevelFromLevelPresent(uint16 level, bool isGuest) const
				{
					if (isGuest == true) {
						std::map< uint16, FamilyPresentInfo>::const_iterator iter = guestMemberLevelTable.begin();
						std::map< uint16, FamilyPresentInfo>::const_iterator end = guestMemberLevelTable.end();
						while (iter != end) {
							if (iter->first > level) {
								return iter->first;
							}
							++iter;
						}
					}
					else {
						std::map< uint16, FamilyPresentInfo>::const_iterator iter = regularMemberLevelTable.begin();
						std::map< uint16, FamilyPresentInfo>::const_iterator end = regularMemberLevelTable.end();
						while (iter != end) {
							if (iter->first > level) {
								return iter->first;
							}
							++iter;
						}
					}
					return 0;
				}
				uint32 FamilyInfoManager::GetNextStoredLevelFromStoredLevelPresent(uint32 level, bool isGuest) const
				{
					if (isGuest == true) {
						std::map< uint32, FamilyPresentInfo>::const_iterator iter = guestMemberStoredLevelTable.begin();
						std::map< uint32, FamilyPresentInfo>::const_iterator end = guestMemberStoredLevelTable.end();
						while (iter != end) {
							if (iter->first > level) {
								return iter->first;
							}
							++iter;
						}
					}
					else {
						std::map< uint32, FamilyPresentInfo>::const_iterator iter = regularMemberStoredLevelTable.begin();
						std::map< uint32, FamilyPresentInfo>::const_iterator end = regularMemberStoredLevelTable.end();
						while (iter != end) {
							if (iter->first > level) {
								return iter->first;
							}
							++iter;
						}
					}
					return 0;
				}
				const FamilyPresentInfo* FamilyInfoManager::GetMemorialDayPresent(uint32 day) const
				{
					size_t max = memorialDayPresents.size();
					for (size_t i = 0; i < max; ++i)
					{
						if (memorialDayPresents[i].Day == day) {
							return &(memorialDayPresents[i].Present);
						}
					}
					return NULL;
				}
				DateTime FamilyInfoManager::GetNextMemorialDayPresentDate(DateTime created, DateTime now) const
				{
					if (created.GetDate() > now.GetDate()) {
						LoggerInstance().Warn(L"GetNextMemorialDayPresentDate : wrong now time : {0}, {1}", created.ToString().c_str(), now.ToString().c_str());
						now = created;
					}
					uint32 day = now.GetDate().GetCumulatedDay() - created.GetDate().GetCumulatedDay();
					size_t max = memorialDayPresents.size();
					for (size_t i = 0; i < max; ++i)
					{
						if (memorialDayPresents[i].Day > day) {
							created.Add(DateTime::Unit::Day, memorialDayPresents[i].Day);
							created.SetTime(DateTime::Time());
							return created;
						}
					}
					return DateTime::Infinite;
				}

				const FamilyPresentInfo* FamilyInfoManager::GetStageClearPresent(uint32 stageInfoHash, uint16 onlineRegularMember, uint16 onlineGuestMember) const
				{
					std::map< uint32, StageClearPresentInfo>::const_iterator iter = stageClearPresentTable.find(stageInfoHash);
					if (iter != stageClearPresentTable.end()) {
						if ((iter->second.RegularMemberOnlineMinCount <= onlineRegularMember) && (iter->second.GuestMemberOnlineMinCount <= onlineGuestMember)) {
							return &(iter->second.Present);
						}
					}
					return NULL;
				}
				const StageClearPresentInfo* FamilyInfoManager::GetStateClearPresentInfo(uint32 stageInfoHash) const
				{
					std::map< uint32, StageClearPresentInfo>::const_iterator iter = stageClearPresentTable.find(stageInfoHash);
					if (iter != stageClearPresentTable.end()) {
						return &(iter->second);
					}
					return NULL;
				}

			}
		}
	}
}
