#include <StageServer/User/Services/FamilyManager.h>
#include <StageServer/User/User.h>
#include <StageServer/Protocol/FromServer.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			const static std::string guestStageLevelUpLogName("family-gift-levelup-g");
			const static std::string memberStageLevelUpLogName("family-gift-levelup-m");
			const static std::string guestStoredLevelUpLogName("family-gift-slevelup-g");
			const static std::string memberStoredLevelUpLogName("family-gift-slevelup-m");
			const static std::string rebirthLogName("family-gift-rebirth");
			const static std::string contributeLogName("family-gift-contribute");
			const static std::string groupPlayTimeLogName("family-gift-playtime");
			const static std::string fishingLogName("family-gift-fishing");
			const static std::string memorialDayLogName("family-gift-memorialday");
			const static std::string stateClearLogName("family-gift-stageclear");

			FamilyManager::FamilyManager(User& owner) 
				: m_Owner(owner)
			{
				m_IsFamily = false;
				m_IsWaitingForInfo = true;

				m_Condition.NextPlayTimeForPersonalPlay = 0xffffffffffffffff;
				m_Condition.ReceivedDateGroupPlay = DateTime::Infinite;
				m_Condition.MemorialDay = DateTime::Infinite;
				m_ConnectedDate = DateTime::Infinite;

				m_Info.CreatedTime = DateTime::Infinite;
				m_Info.Serial = 0;

				m_MyInfo.MemberSerial = 0;
				m_MyInfo.StageLevel = 0;
				m_MyInfo.PvpLevel = 0;
				m_MyInfo.IsGuest = false;
				m_MyInfo.IsOnline = false;
				m_MyInfo.PlayTime = 0;

				m_OnlineGuestCount = 0;
				m_OnlineRegularCount = 0;
			}
			void FamilyManager::Init(const std::wstring& characterName)
			{
				AutoLock lock(m_Mtx);
				Clear();
				m_MyInfo.CharacterName = characterName;
				m_ConnectedDate = DateTime::Now();
				RequestDBFamilyInfoForInit();
			}
			void FamilyManager::RequestDBFamilyInfoForInit()
			{
				Net::Api api("Family/JoinedInfo");
				api << m_Owner.GetSerial();
				api.GetAsync(
					[&](const Net::Answer& result) {
						if (result.errorCode == 0) {
							SetFamilyInfo(result.resultObject);
						}
						else
							Logger::GetInstance().Error(L"Could not parse FamilyInfo for the user@{0}", m_Owner.GetCharacterName());
					});
			}
			void FamilyManager::RequestDBUpdateMemorialDay()
			{
				//Scopped expectected to be locked by caller.
				if (IsFamily()) {
					Net::Api request("Family/UpdateMemorialDay");
					//familyId|MemorialDate
					request << m_Info.Serial << m_Condition.MemorialDay;
					request.GetAsync();
				}
			}
			void FamilyManager::RequestDBLeave()
			{
			}
			bool FamilyManager::IsFamily() const
			{
				return m_IsFamily;
			}
			void FamilyManager::ResetFamilyPassive()
			{
			}
			void FamilyManager::SetFamilyInfo(const json& data)
			{
				if (data.is_null())
					return;
				auto& self = data["self"];
				Protocol::FromServer::Family::Info result;
				{
					AutoLock lock(m_Mtx);
					//Populate the members
					for (auto& member : data["familyMembers"]) {
						m_Members.push_back(Family::MemberInfo());
						auto& info = m_Members.back();
						member["id"].get_to(info.MemberSerial); // familyMember ID NOT CharacterSerial !!!!!!!!!!!!!!!!!!!
						info.CharacterName = StringUtil::ToUnicode(member["characterName"].get<std::string>());
						info.Class = static_cast<XRated::Constants::ClassType>(member["classNumber"].get<uint8>());
						member["stageLevel"].get_to(info.StageLevel);
						member["pvpLevel"].get_to(info.PvpLevel);
						member["lastLoggedDate"].get_to(info.LastLoggedDate);
						member["isGuest"].get_to(info.IsGuest);
						member["isOnline"].get_to(info.IsOnline);
						member["playTime"].get_to(info.PlayTime);
						member["joinedDate"].get_to(info.JoinedDate);
						//populate the counts.
						if (info.IsOnline) {
							if (info.IsGuest)
								m_OnlineGuestCount++;
						}
						else
							m_OnlineRegularCount++;
					}
					m_IsWaitingForInfo = false;
					auto now = DateTime::Now();
					if (!IsFamily()) {
						m_ConnectedDate = now;
						self["playTime"].get_to(m_MyInfo.PlayTime);
						m_Condition.NextPlayTimeForPersonalPlay = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextPersonalPlayPresent(m_MyInfo.PlayTime);
						self["createDate"].get_to(m_Condition.MemorialDay);
					}
					else {
						Protocol::FromServer::Family::TakePresentResult present;
						present.Param = 0;
						present.Type = XRated::Family::PresentType::PersonalPlay;
						present.IsSuccess = SendPlayTimePresent(now, present.receiveMembers);
						if (present.IsSuccess == true)
						{
							present.Condition = m_Condition;
							m_Owner.Send(present);
						}
					}

					m_IsFamily = true;
					m_MyInfo.CharacterName = m_Owner.GetCharacterName();
					if (m_Owner.GetPlayer() == nullptr) {
						m_MyInfo.Class = m_Owner.GetInitialPlayerData()->type;
						m_MyInfo.StageLevel = m_Owner.GetInitialPlayerData()->level;
						m_MyInfo.PvpLevel = m_Owner.GetInitialPlayerData()->pvpLevel;
					}
					else {
						m_MyInfo.Class = m_Owner.GetClassType();
						m_MyInfo.StageLevel = m_Owner.GetLevel();
						m_MyInfo.PvpLevel = m_Owner.GetPvpLevel();
					}

					self["familyId"].get_to(m_Info.Serial);
					self["createDate"].get_to(m_Info.CreatedTime);

					self["accountId"].get_to(m_MyInfo.MemberSerial);
					self["isGuest"].get_to(m_MyInfo.IsGuest);
					self["isOnline"].get_to(m_MyInfo.IsOnline);
					
					self["expireGroupGift"].get_to(m_Condition.ReceivedDateGroupPlay);
					self["joinedDate"].get_to(m_JoinedDate);
					if (m_Condition.MemorialDay < m_JoinedDate) {
						m_Condition.MemorialDay = Database::DatabaseInstance().InfoCollections.FamilyInfos.GetNextMemorialDayPresentDate(m_Info.CreatedTime, m_JoinedDate);
						RequestDBUpdateMemorialDay();
					}

					if (m_Members.size() < 2) {
						RequestDBLeave();
						return;
					}
					result.FamilyInfo = m_Info;
					result.Members = m_Members;
					result.Condition = m_Condition;
				}
				ResetFamilyPassive();
				m_Owner.Send(result);
			}
			bool FamilyManager::SendPlayTimePresent(DateTime now, std::vector<Family::FamilyMemberSerial>& receiveMembers)
			{
				return true;
			}
			void FamilyManager::Clear()
			{
			}
		}
	}
}