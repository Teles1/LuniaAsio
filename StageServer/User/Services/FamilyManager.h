#pragma once
#include <Core/GameConstants.h>
#include <Network/Api/Api.h>
#include <StageServer/Protocol/FromServer.h>
#include <StageServer/Protocol/ToServer.h>
#include <Info/Info.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class FamilyManager
			{

			public:
				void Init(const std::wstring& characterName);
				void Release();
				bool IsFamily() const;
				bool IsInvited() const;

				bool IsGuest() const;

				Protocol::FromServer::Family::InviteResult::Type Invite(const std::wstring& inviter);
				Protocol::FromServer::Family::InviteResult::Type Invite(const std::wstring& inviter, XRated::Family::FamilySerial familySerial, DateTime createdDate);

				void LevelUp(uint16 level);
				void AcquiredFishItem(uint32 itemHash);
				void Rebirthed();
				void StoredLevelUp(uint32 storedLevel);
				void MissionClear(bool success, XRated::Database::Info::StageGroup* group, uint16 accessLevel);

				void ResetFamilyPassive();

			public:
				FamilyManager(User& user);
				~FamilyManager();
			public:
				void Dispatch(UserSharedPtr, Protocol::ToServer::Family::Invite& packet);
				void Dispatch(UserSharedPtr, Protocol::ToServer::Family::Leave& packet);
				void Dispatch(UserSharedPtr, Protocol::ToServer::Family::InviteAnwer& packet);
				void Dispatch(UserSharedPtr, Protocol::ToServer::Family::RefreshInfo& packet);
				void Dispatch(UserSharedPtr, Protocol::ToServer::Family::RequestPresent& packet);
				void Dispatch(UserSharedPtr, Protocol::ToServer::Family::Kick& packet);
			private:
				void RequestDBFamilyInfoForInit();
				void RequestDBFamilyInfoForRefresh();
				void RequestDBJoin();
				void RequestDBLeave(bool isPenalty);
				void RequestDBTerminate();
				void RequestDBUpdateMemorialDay();


			private:
				//DB Response
				void DBFamilyInfoForInit(const UserSharedPtr& user, const Net::Answer& answer);
				void DBFamilyInfoForRefresh(const UserSharedPtr& user, const Net::Answer& answer);
				void DBCreated(const UserSharedPtr& user, const Net::Answer& answer);
				void DBJoined(const UserSharedPtr& user, const Net::Answer& answer);
				void DBLeaved(const UserSharedPtr& user, const Net::Answer& answer);
				void DBKicked(const UserSharedPtr& user, const Net::Answer& answer);

				void SetFamilyInfo(const Net::Answer& answer, bool isRefresh);
			private:
				void Clear();

				uint32 SendMemorialDayPresent(DateTime now, std::vector<XRated::Family::FamilyMemberSerial>& receiveMembers);
				bool SendPlayTimePresent(DateTime now, std::vector<XRated::Family::FamilyMemberSerial>& receiveMembers);
				void SendPresentMail(const std::string& logName, const Database::Info::FamilyPresentInfo& info, std::vector< XRated::Family::FamilyMemberSerial >& sendedMembers);
				bool SendPresentMail(const std::string& logName, const XRated::Family::FamilyMemberSerial receiverMemberSerial, const std::wstring& receiver
					, const std::wstring& sender, const std::wstring& title, const std::wstring& text, const Database::Info::FamilyPresentInfo::Present& present, const DateTime current, bool isGuest);
			private:
				std::mutex cs;

				User& owner;

				bool							isFamily;
				XRated::Family::Info			info;
				XRated::Family::MemberInfo		myInfo;
				XRated::Family::RewardCondition	condition;
				DateTime						joinedDate;
				DateTime						connectedDate;

				typedef std::vector<XRated::Family::MemberInfo>	FamilyMemberList;
				FamilyMemberList		members;

				std::wstring inviteOwnerName;

				XRated::Family::FamilySerial		inviteFamilyId;
				DateTime						inviteFamilyCreatedDate;

				bool	isFamilyInfoWait;

				uint32	onlineGuestCount;
				uint32	onlineRegularCount;
			};

		}
	}
}
