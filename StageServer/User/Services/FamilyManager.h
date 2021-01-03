#pragma once
#include <Core/GameConstants.h>
#include <Network/Api/Api.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class FamilyManager {
			public:
				FamilyManager(User& owner);
				void Init(const std::wstring& characterName);
				bool IsFamily() const;
				void ResetFamilyPassive();
			private:
				void SetFamilyInfo(const json& data);
				bool SendPlayTimePresent(DateTime now, std::vector<Family::FamilyMemberSerial>& receiveMembers);
				void Clear();
			private: //DBRelated
				void RequestDBFamilyInfoForInit();
				void RequestDBUpdateMemorialDay();
				void RequestDBLeave();
			private:
				User& m_Owner;
				std::mutex m_Mtx;
				Family::Info			m_Info;
				Family::MemberInfo		m_MyInfo;
				typedef std::vector<XRated::Family::MemberInfo>	FamilyMemberList;
				FamilyMemberList		m_Members;
				Family::RewardCondition	m_Condition;
				DateTime				m_ConnectedDate;
				DateTime				m_JoinedDate;
				uint8					m_OnlineGuestCount;
				uint8					m_OnlineRegularCount;
				bool					m_IsWaitingForInfo;
				bool					m_IsFamily;
			};
		}
	}
}
