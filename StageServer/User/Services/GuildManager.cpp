#include "GuildManager.h"
#include <StageServer/User/UserManager.h>
#include <StageServer/User/User.h>
#include <Network/Api/Api.h>
#include <Core/Utils/TimeSpan.h>
namespace Lunia {
	namespace Net {
		
	}
	namespace XRated {
		namespace StageServer {
			
			/**
			GuildManager
			*/
			GuildManager::GuildManager()
				: CumulativeTax(0), debugExpTimeFactor(0.0f)
				, CreationFee(static_cast<int>(Database::DatabaseInstance().InfoCollections.GuildInfos.GetGeneral().CreationFee))
				, MinimalCreationLevel(Database::DatabaseInstance().InfoCollections.GuildInfos.GetGeneral().MinimalCreationPlayerLevel)
			{
				debugExpTimeFactor = ConfigInstance().Get("ExpTimeFactor", 0.0f);
			}
			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::Create& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());


				/* validate - guild creation needs money and level */
				if (user->GetMoney() < CreationFee || user->GetPlayer()->GetLevel() < MinimalCreationLevel)
				{
					/* TODO : Make critical */
				}
				else
				{
					/* fee processing */
					user->SetMoney(user->GetMoney() - CreationFee);
					//' request  : guild name, guild id, character name(guild master), intro message, stageLevel, pvpLevel, exp
					Net::Api request("Guild_v3.Create");
					request << packet.GuildName << packet.Alias << user->GetSerial() << packet.Message;
					request.GetAsync(this, &GuildManager::Created, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::Invite& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				auto target = UserManagerInstance().GetUserByName(packet.TargetCharacterName);

				Protocol::FromServer::AllMGuild::Invite response;
				if (user->IsPartOfGuild() == false)
				{
					/* TODO : Make critical */
					return;
				}
				else if (user->IsPartOfGuild() == true &&
					user->GetGuildInfo().MemberCount >= Database::DatabaseInstance().InfoCollections.GuildInfos.GetMaxGuildUserPerLevel(user->GetGuildInfo().GuildLevel))
				{
					Protocol::FromServer::AllMGuild::Join response;
					response.Result = Protocol::FromServer::AllMGuild::Join::Results::TooManyGuildMember;
					user->Send(response);

					return;
				}
				else if (target == NULL)
				{
					response.Result = Protocol::FromServer::AllMGuild::Invite::InvalidTarget;
				}
				else if (target->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}
				else if (target->IsPartOfGuild())
				{
					response.Result = Protocol::FromServer::AllMGuild::Invite::AlreadyJoined;
				}
				else if (!user->GetGuildInfo().IsAbleToInvite())
				{
					response.Result = Protocol::FromServer::AllMGuild::Invite::NoRight;
				}
				else // finally
				{
					response.Result = Protocol::FromServer::AllMGuild::Invite::Ok;

					/* send invitation */
					Protocol::FromServer::AllMGuild::InvitedFrom invitation;
					invitation.GuildId = user->GetGuildInfo().GuildId;
					invitation.GuildName = user->GetGuildInfo().GuildName;
					invitation.InvitorCharacterName = user->GetName();
					target->Send(invitation);
				}
				user->Send(response);
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::InviteReply& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				auto target = UserManagerInstance().GetUserByName(packet.TargetCharacterName);

				AutoLock lock(user->GetSyncObject());
				
				if (target && target->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Protocol::FromServer::AllMGuild::InviteReply response;
				if (target != NULL) {
					response.Result = (Protocol::FromServer::AllMGuild::InviteReply::Results)packet.ReplyCode;
					response.TargetCharacterName = packet.TargetCharacterName;
					target->Send(response);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::Join& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				if (user->IsPartOfGuild())
				{
					Protocol::FromServer::AllMGuild::Join response;
					response.Result = Protocol::FromServer::AllMGuild::Join::Results::AlreadyJoined;
					user->Send(response);
				}
				else
				{
					Net::Api request("Guild_v3.Join");
					request << packet.GuildId << user->GetName() << 4;
					request.GetAsync(this, &GuildManager::Joined, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::Kick& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				auto target = UserManagerInstance().GetUserByName(packet.TargetCharacterName);

				AutoLock lock(user->GetSyncObject());
				
				if (target && target->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (user->IsPartOfGuild() == false
					|| !user->GetGuildInfo().IsAbleToKick()) {//  should be master
					/* TODO : Make critical */
				}
				else {
					Net::Api request("Guild_v3.Kick");
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << packet.TargetCharacterName;
					request.GetAsync(this, &GuildManager::Kicked, user);

				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::Leave& /*packet*/)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				
				if (user->IsPartOfGuild() == false
					|| user->GetGuildInfo().MasterName == user->GetName()/* master cannot leave */)
				{
					/* TODO : Make critical */
				}
				else
				{
					//' request  : guild oid, character name, kicked
					Net::Api request("Guild_v3.Leave");
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId;
					request.GetAsync(this, &GuildManager::Left, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ListMembers& /*packet*/)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				if (user->IsPartOfGuild() == false)
				{
					LoggerInstance().Error("Invalid guild state. {}/0x{}", user->IsPartOfGuild(), user->GetGuildInfo().GuildId);
				}
				else
				{
					//' request  : guild oid, character name, kicked
					Net::Api request("Guild_v3.ListMembers");
					//request : guildid
					request << user->GetGuildInfo().GuildId;
					request.GetAsync(this, &GuildManager::MemberListed, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::Remove& /*packet*/)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				if (user->IsPartOfGuild() == false ||
					user->GetGuildInfo().MasterName != user->GetName() ||
					user->GetGuildInfo().MemberCount > 1)
				{
					/* TODO : Make critical */
					LoggerInstance().Error(L"Wrong AllMGuild::Remove packet : IsJoined: {}, MasterName: {}, userName: {}, MemberCount: {}", user->IsPartOfGuild() ? L"true" : L"false", user->GetGuildInfo().MasterName.c_str(), user->GetName().c_str(), user->GetGuildInfo().MemberCount);
				}
				else
				{
					//' request  : guild oid
					Net::Api request("Guild_v3.Remove");
					request << user->GetGuildInfo().GuildId << user->GetName();
					request.GetAsync(this, &GuildManager::Removed, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ValidateName& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				/* TODO : simple validate */

				if (user->IsPartOfGuild()) //  should be unjoined user
				{
					/* TODO : Make critical */
				}
				else
				{
					//' request  : guild name
					Net::Api request("Guild_v3.CheckName");
					request << packet.Name;
					request.GetAsync(this, &GuildManager::NameValidated, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ValidateAlias& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				/* TODO : simple validate */

				if (user->IsPartOfGuild()) //  should be unjoined user
				{
					/* TODO : Make critical */
				}
				else
				{
					//' request  : guild name
					Net::Api request("Guild_v3.CheckAlias");
					request << packet.Alias;
					request.GetAsync(this, &GuildManager::AliasValidated, user);
				}
			}
			
			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::AddExp& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false) {
					user->CriticalError("user have not guild info", false);
					return;
				}
				DateTime now(DateTime::Now());
				std::pair<float, uint32> calculated;
				// debugExpTimeFactor?? debug?? ??????. by kwind.
				if (debugExpTimeFactor > 1.0f) {
					float time = 0.0f;
					const static int secondOfDay = 60 * 60 * 24;//(60?? * 60?? * 24?ð?/??? ??? ????? ????).
					time += now.GetTime().GetCumulatedSec() - user->GetGuildInfo().StartedTimeToPlay.GetTime().GetCumulatedSec();
					time += (now.GetDate().GetCumulatedDay() - user->GetGuildInfo().StartedTimeToPlay.GetDate().GetCumulatedDay()) * secondOfDay;
					time *= debugExpTimeFactor;
					DateTime temp(now);
					temp.Add(DateTime::Unit::Second, (uint32)time);
					calculated = Database::DatabaseInstance().InfoCollections.GuildInfos.CalculdateCurrentUserExp(user->GetGuildInfo().StartedTimeToPlay, temp, user->GetGuildInfo().MyPlayTime);
				}
				else {
					calculated = Database::DatabaseInstance().InfoCollections.GuildInfos.CalculdateCurrentUserExp(user->GetGuildInfo().StartedTimeToPlay, now, user->GetGuildInfo().MyPlayTime);
				}
				if (calculated.second > 0) {

					std::pair<bool, uint32> nextLevelExp = Database::DatabaseInstance().InfoCollections.GuildInfos.GetNextLevelExp(user->GetGuildInfo().GuildLevel);
					if (nextLevelExp.first == true) {
						Net::Api request("Guild_v3.ExpUp");
						request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << calculated.second;
						request.GetAsync(this, &GuildManager::AddedGuildExp, user);
						return;
					}
					else {
						user->GetGuildInfo().MyPlayTime = 0.0f;
					}
				}
				Protocol::FromServer::AllMGuild::AddedGuildExp response;
				response.Result = Protocol::FromServer::AllMGuild::AddedGuildExp::Results::Failed;
				response.GuildLevel = user->GetGuildInfo().GuildLevel;
				response.GuildExp = user->GetGuildInfo().GuildExp;
				response.PlayTime = user->GetGuildInfo().MyPlayTime;
				response.StartedTimeToPlay = user->GetGuildInfo().StartedTimeToPlay;
				response.IsByItem = false;
				user->Send(response);
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::AddMaintenancePoint& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false) {
					user->CriticalError("user have not guild info", false);
					return;
				}
				uint32 Point = user->GetItemCount(packet.ItemHash);
				if (packet.Point < Point)
					Point = packet.Point;
				if (user->RemoveItems(packet.ItemHash, Point) == true)
				{
					Net::Api request("Guild_v3.Maintain");
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << Point;
					request.GetAsync(this, &GuildManager::AddedGuildExp, user);
				}
				return;
			}
			
			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ReflashGuildLevel& packet)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				int guildId;
				{
					//db?? ???? ????? ???
					AutoLock lock(user->GetSyncObject());
					if (user->IsPartOfGuild() == false) {
						user->CriticalError("user have not guild info", false);
						return;
					}
					guildId = user->GetGuildInfo().GuildId;
				}

				RequestReflashGuildLevel(user, guildId, packet.level);
			}
			
			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::GetTax& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				if (user->IsPartOfGuild() == false) {
					/* TODO : Make critical */
				}
				else if (user->GetGuildInfo().MasterName != user->GetName())
				{
					Protocol::FromServer::AllMGuild::TaxGained response;
					response.Result = Protocol::FromServer::AllMGuild::TaxGained::NotMaster;
					response.TaxPayDate = DateTime::Now();
					user->Send(response);
				}
				else if (user->GetGuildInfo().Tax < packet.tax)
				{
					Protocol::FromServer::AllMGuild::TaxGained response;
					response.Result = Protocol::FromServer::AllMGuild::TaxGained::NotEnoughTax;
					response.TaxPayDate = DateTime::Now();
					user->Send(response);
				}
				else if (user->GetGuildInfo().TaxPayDay.GetDate().GetCumulatedDay() > DateTime::Now().GetDate().GetCumulatedDay())
				{
					Protocol::FromServer::AllMGuild::TaxGained response;
					response.Result = Protocol::FromServer::AllMGuild::TaxGained::NotElapsedPayDate;
					response.TaxPayDate = DateTime::Now();
					user->Send(response);
				}
				else if (user->GetMoney() + packet.tax > XRated::Constants::GoldMaxLimit)
				{
					Protocol::FromServer::AllMGuild::TaxGained response;
					response.Result = Protocol::FromServer::AllMGuild::TaxGained::OverMaxGold;
					response.TaxPayDate = DateTime::Now();
					user->Send(response);
				}
				else
				{
					Net::Api request("Guild_v3.DrawTax");

					//request : guildId, guildMemberId, tax
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << packet.tax;
					request.GetAsync(this, &GuildManager::TaxGained, user);
				}
			}
			
			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ReflashGuildShopItem& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				int guildId = 0;
				{
					AutoLock lock(user->GetSyncObject());
					if (user->IsPartOfGuild() == false) {
						user->CriticalError("user have not guild info", false);
						return;
					}
					guildId = user->GetGuildInfo().GuildId;
				}
				if (GetGuildsUserManager().IsExistGuildShopItem(guildId, packet.Item.ItemHash, packet.Item.ExpiredDate) == false) {
					//request guild item list.
					if (GetGuildsUserManager().SetLastDbRequestedGuildShopItem(guildId, packet.Item) == true) {
						//request db.
						Net::Api request("Guild_v3.ListGuildShopProduct");
						request << guildId;
						request.GetAsync(this, &GuildManager::ReflashedListGuildShopItem, user);
					}
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::MessageChange& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false) {
					/* TODO : Make critical */
				}
				else {

					Net::Api request("Guild_v3.ChangeMessage");
					//request : guildId, newMessage, guildMemberId
					request << user->GetGuildInfo().GuildId << packet.Message << user->GetGuildInfo().GuildMemberId;
					request.GetAsync(this, &GuildManager::MessageChanged, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGradeName& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false) {
					/* TODO : Make critical */
				}
				else {
					Net::Api request("Guild_v3.ChangeGradeName");
					request << user->GetGuildInfo().GuildId << packet.TargetGrade << user->GetGuildInfo().GuildMemberId << packet.TargetGradeName;
					request.GetAsync(this, &GuildManager::GradeNameChanged, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGradeAuth& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				
				if (user->IsPartOfGuild() == false) {
					/* TODO : Make critical */
				}
				else {
					Net::Api request("Guild_v3.ChangeGradeAuthority");
					// request : guildId, grade, guildMemberId, authority
					request << user->GetGuildInfo().GuildId << packet.TargetGrade << user->GetGuildInfo().GuildMemberId << packet.Authority;
					request.GetAsync(this, &GuildManager::GradeAuthChanged, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGrade& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				//usermanager?? ?????? ???? user?? Lock?? ????????? dead lock?????? ??? ?????? user?? Lock?? ??????? ó??.
				auto target = UserManagerInstance().GetUserByName(packet.TargetCharacterName);

				AutoLock lock(user->GetSyncObject());

				if (target && target->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if ((user->IsPartOfGuild() == false) ||
					packet.Grade == 0 /*Unable to change grade to 0*/) {
					/* TODO : Make critical */
				}
				else {
					Net::Api request("Guild_v3.ChangeMemberGrade");
					// request : guildId, guildMemberId, characterName_target, grade_target
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << packet.TargetCharacterName << packet.Grade;
					request.GetAsync(this, &GuildManager::GradeChanged, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGuildMaster& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				//usermanager?? ?????? ???? user?? Lock?? ????????? dead lock?????? ??? ?????? user?? Lock?? ??????? ó??.
				auto target = UserManagerInstance().GetUserByName(packet.TargetCharacterName);

				AutoLock lock(user->GetSyncObject());

				if (target && target->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!user->IsPartOfGuild() ||
					user->GetGuildInfo().MasterName != user->GetName() ||
					target == NULL /*Can make only online user to guildmaster */) {
					/* TODO : Make critical */
				}
				else {
					Net::Api request("Guild_v3.ChangeMaster");
					// request guildid, guildMemberId, characterName_Target
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << packet.TargetCharacterName;
					request.GetAsync(this, &GuildManager::GuildMasterChanged, user);
				}
			}

			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::GetMyInfo& /*packet*/)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());

				if (user->IsPartOfGuild())
					RequestGuildInfo(user);
			}
			
			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::RefreshMyInfo& /*packet*/)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				
				if (!user->IsPartOfGuild()) {
					/* TODO : Make critical */
				}
				else {
					Net::Api request("Guild_v3.JoinedInfo");
					//request : characterName
					request << user->GetSerial();
					request.GetAsync(this, &GuildManager::MyInfoRefreshed, user);
				}
			}

			void GuildManager::RequestValidGuildInfo(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Net::Api request("Guild_v3.JoinedInfo");
				request << user->GetSerial();
				request.GetAsync(this, &GuildManager::MyInfoVaild, user);
			}

			void GuildManager::RequestGuildInfo(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Net::Api request("Guild_v3.JoinedInfo");
				request << user->GetSerial();
				request.GetAsync(this, &GuildManager::MyInfoRefreshed, user);
			}
			
			void GuildManager::RequestGuildInfoForJoinUser(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					UserManagerInstance().SendCurrentStageEvents(user);
					return;
				}

				user->SetRequestedInitGuildInfo(true);
				Net::Api request("Guild_v3.JoinedInfo");
				request << user->GetSerial();
				request.GetAsync(this, &GuildManager::SetPlayer, user);
			}

			void GuildManager::RequestGuildInfo(UserSharedPtr user, uint32 /*guildOid*/)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Net::Api request("Guild_v3.JoinedInfo");
				//request : characterName
				request << user->GetSerial();
				request.GetAsync(this, &GuildManager::MyInfoRefreshed, user);
			}

			void GuildManager::GuildPointUp(UserSharedPtr user, uint32 point)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false)
				{
					/* TODO : Make critical */
				}
				else
				{
					Net::Api request("Guild_v3.Maintain");
					//request : guildid, guildMemberId, guildPoint
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << point;
					request.GetAsync(this, &GuildManager::AddedGuildPoint, user);
				}
			}

			void GuildManager::GuildExpUp(UserSharedPtr user, uint32 exp)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false)
				{
					/* TODO : Make critical */
				}
				else
				{
					Net::Api request("Guild_v3.ExpUp");
					//request : guildid, guildMemberId, guildPoint
					request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << exp;
					request.GetAsync(this, &GuildManager::AddedGuildExpByItem, user);
				}
			}

			void GuildManager::IncreaseGuildRankingPoint(UserSharedPtr user, uint32 hours)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false)
				{
					/* TODO : Make critical */
				}
				else
				{
					Net::Api request("Guild_v3.AddGuildPlayTime");
					//request : guildid, guildMemberId, guildPoint
					const uint32 secondsInOneHour = 3600;
					request << user->GetGuildInfo().GuildId << (hours * secondsInOneHour);
					request.GetAsync(this, &GuildManager::IncresedGuildRankPointByItem, user);
				}
			}

			void GuildManager::RequestAddItemToGuildShop(UserSharedPtr user, uint32 addItemHash, uint8 usableGuildLevel, int day)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				//DB?? item??? ??û???.
				AutoLock lock(user->GetSyncObject());
				if (user->IsPartOfGuild() == false) {
					return;
				}

				Net::Api request("Guild_v3.AddGuildShopProduct");
				//request : guildId,guildMemberId,itemHash,usableGuildLevel,days
				request << user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildMemberId << addItemHash << usableGuildLevel << day;
				request.GetAsync(this, &GuildManager::AddedItemToGuildShop, user);
			}
			
			void GuildManager::RequestReflashGuildShopItemList(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (user->IsPartOfGuild() == false) {
					return;
				}

				Net::Api request("Guild_v3.ListGuildShopProduct");
				request << user->GetGuildInfo().GuildId;
				request.GetAsync(this, &GuildManager::ListGuildShopItem, user);
			}
			
			void GuildManager::RequestGuildRankList(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Net::Api request("Guild_v3.ListRank");
				request.GetAsync(this, &GuildManager::GuildRankingListed, user);
			}

			void GuildManager::SaveTax(float tax)
			{
				AutoLock lock(cs);
				if (tax > 0)
				{
					CumulativeTax += tax;
				}
			}
			
			void GuildManager::UpdateTaxToDB(UserSharedPtr user)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (static_cast<int>(CumulativeTax) > 0)	///by kpongky( 09.07.29 ) BMS 6902
				{
					Net::Api request("Guild_v3.PutTax");
					request << static_cast<uint32>(CumulativeTax);	///by kpongky( 09.07.29 ) use static_cast than c-cast
					request.GetAsync(this, &GuildManager::TaxUpdated, user);
				}
				else
				{
					if (CumulativeTax < 0.0f)
					{
						LoggerInstance().Error("wrong UpdateTaxToDB : CumulativeTax - {0}", CumulativeTax);
					}
				}
			}
			
			void GuildManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AllMGuild::ReflashGuildProfitDate& packet)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				int guildId;
				{
					AutoLock lock(user->GetSyncObject());
					if (user->IsPartOfGuild() == false) {
						/* TODO : Make critical */
						return;
					}
					if ((user->GetGuildInfo().ShopOpenDate == packet.OpenDate) && (user->GetGuildInfo().ShopCloseDate >= packet.CloseDate)) {
						return;
					}
					guildId = user->GetGuildInfo().GuildId;
				}
				if (GetGuildsUserManager().SetLastDbRequestedGuildProfitDate(guildId, packet.OpenDate, packet.CloseDate) == true) {
					Net::Api request("Guild_v3.JoinedInfo");
					//request : characterName
					request << user->GetSerial();
					request.GetAsync(this, &GuildManager::ReflashedGuildProfitDate, user);
				}
			}

			void GuildManager::InitCumulativeTax()
			{
				AutoLock lock(cs);
				CumulativeTax = 0;
			}
			
			void GuildManager::TaxUpdated(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer)
				{
					switch (answer.errorCode)
					{
						//1st rank guild not exists
					case 1:
						//2 : guild not exists
					case 2:
						InitCumulativeTax();
						break;
					}
				}
				else
				{
					uint32 changedAmountCumulativeTax = 0;
					{
						AutoLock lock(cs);
						answer.get_to("changedAmountCumulativeTax", changedAmountCumulativeTax);
						CumulativeTax -= changedAmountCumulativeTax;
						if (CumulativeTax < 0.0f)
						{
							LoggerInstance().Error("TaxUpdated : wrong DBreturn, CumulativeTax = {}, changedAmountCumulativeTax = {}", CumulativeTax, changedAmountCumulativeTax);
							CumulativeTax = 0;
						}
					}


					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-put-tax", L"", Http::Logger::Parameter()
						<< StringUtil::ToInt((*packet)[0]) << StringUtil::To<std::wstring>((*packet)[1]).c_str() << changedAmountCumulativeTax);*/


					if (user)
					{
						AutoLock userLock(user->GetSyncObject());
						if (user->IsPartOfGuild() == true)
						{
							if (user->GetGuildInfo().GuildId == answer.at("id").get<int>())
							{
								Protocol::FromServer::AllMGuild::TaxChanged response;
								answer.get_to("tax", response.Tax);
								user->GetGuildInfo().Tax = response.Tax;
								user->Send(response);
							}
						}
					}
				}
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// AllM guild DB response
			void GuildManager::Created(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
					return;
				//' response : guild oid
				Protocol::FromServer::AllMGuild::Create response;
				if (answer)
				{
					response.Result = Protocol::FromServer::AllMGuild::Create::Ok;
					answer.get_to("id", response.guildId);

					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-create", user->GetName(), Http::Logger::Parameter()
						<< response.guildId << StringUtil::ToUnicode((*packet)[1]));*/

					RequestGuildInfoForJoinUser(user); // request guild information to broadcast SetPlayer 
				}
				else
				{
					/* recover the fee(rollback) */
					user->SetMoney(user->GetMoney() + CreationFee);

					response.Result = static_cast<Protocol::FromServer::AllMGuild::Create::Results>(answer.errorCode);
				}
				user->Send(response);
				/*if (user->achievement.IsConnected()) {
					Net::Protocol::Achievement::ServerProtocol::GuildActionChanged packet;
					packet.characterName = user->GetName();
					packet.Action = Net::Protocol::Achievement::ServerProtocol::GuildActionChanged::Action::Guild_Create;
					user->achievement.Send(packet);
				}*/
			}

			void GuildManager::Joined(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}
				//' response : guild oid
				Protocol::FromServer::AllMGuild::Join response;
				if (!answer)
					response.Result = static_cast<Protocol::FromServer::AllMGuild::Join::Results>(answer.errorCode);
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::Join::Ok;
					answer.get_to("id", response.guildId);

					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-joined", user->GetName(), Http::Logger::Parameter()
						<< response.guildId << StringUtil::ToUnicode((*packet)[1]));*/

					RequestGuildInfoForJoinUser(user); // request guild information to broadcast SetPlayer 
				}
				user->Send(response);
				/*if (user->achievement.IsConnected()) {
					Net::Protocol::Achievement::ServerProtocol::GuildActionChanged packet;
					packet.characterName = user->GetName();
					packet.Action = Net::Protocol::Achievement::ServerProtocol::GuildActionChanged::Action::Guild_Joined;
					user->achievement.Send(packet);
				}*/
			}

			void GuildManager::Left(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}
				//' response : Leave
				Protocol::FromServer::AllMGuild::Leave response;
				if (!answer)
				{
					AutoLock lock(user->GetSyncObject());
					response.Result = Protocol::FromServer::AllMGuild::Leave::Failed;
					user->Send(response);
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::Leave::Ok;
					response.Kicked = false;
					response.Serial = user->GetSerial();
					/*if (user->achievement.IsConnected()) {
						Net::Protocol::Achievement::ServerProtocol::GuildActionChanged packet;
						packet.characterName = user->GetName();
						packet.Action = Net::Protocol::Achievement::ServerProtocol::GuildActionChanged::Action::Guild_Leave;
						user->achievement.Send(packet);
					}*/
					user->SendToAll(response); // broadcasting

					// Guild User Manager???? ????? ???? ?????? ??????(user??Lock?? ?? ???¿??? GuildUsersManager?? ?????? ????).
					if (user->IsPartOfGuild() == true) {
						// guild info?? ???? LeaveUser?? ???? ???.
						GetGuildsUserManager().LeavedUser(user);
					}
					//Logging
					{
						AutoLock lock(user->GetSyncObject());
						std::wstring guildName;
						int guildId = 0;
						if (user->IsPartOfGuild() == true) {
							guildName = user->GetGuildInfo().GuildName;
							guildId = user->GetGuildInfo().GuildId;
						}
						else {
							guildName = L"Unknown";
						}

						/*Logger().Write(IAdmLogger::ImportantLogger, "guild-leave", user->GetName(), Http::Logger::Parameter()
							<< guildId << guildName.c_str());*/


							/* post processing */
						user->ClearGuildInfo();

						user->RemoveGuildAdvantage();
					}

				}
				
			}

			void GuildManager::Kicked(const UserSharedPtr& user, Net::Answer& answer)
			{
				//' response : kicked
				if (!answer)
				{
					AutoLock lock(user->GetSyncObject());
					Protocol::FromServer::AllMGuild::Kick response;
					response.Result = static_cast<Protocol::FromServer::AllMGuild::Kick::Results>(answer.errorCode);
					user->Send(response);
				}
				else
				{
					auto target = UserManagerInstance().GetUserBySerial(answer.at("characterId").get<uint32>());

					if (target) { //If there is user kicked from guild, broad cast message.

						if (target->GetCharacterStateFlags().IsGuestID)
						{
							return;
						}

						Protocol::FromServer::AllMGuild::Leave response;
						response.Result = Protocol::FromServer::AllMGuild::Leave::Ok;
						response.Kicked = true;
						response.Serial = target->GetSerial();
						target->SendToAll(response);

						if (target->IsPartOfGuild() == true) {
							GetGuildsUserManager().LeavedUser(target);
						}

						target->ClearGuildInfo();

						target->RemoveGuildAdvantage();

					}

					Protocol::FromServer::AllMGuild::Kick response;
					response.Result = Protocol::FromServer::AllMGuild::Kick::Results::Ok;
					user->Send(response);

					//Logging
					AutoLock lock(user->GetSyncObject());
					std::wstring guildName;
					if (user->IsPartOfGuild() == true)
						guildName = user->GetGuildInfo().GuildName;
					else
						guildName = L"Unknown";

					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-kick", (StringUtil::ToUnicode((*packet)[0])), Http::Logger::Parameter()
						<< user->GetGuildInfo().GuildId << guildName.c_str());*/

				}
			}

			void GuildManager::Removed(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
					return;
				Protocol::FromServer::AllMGuild::Remove response;
				if (!answer)
				{
					//response.Result = Protocol::FromServer::AllMGuild::Remove::Failed;
					response.Result = static_cast<Protocol::FromServer::AllMGuild::Remove::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::Remove::Ok;

					Protocol::FromServer::AllMGuild::Leave broadcast;
					broadcast.Result = Protocol::FromServer::AllMGuild::Leave::Ok;
					broadcast.Kicked = false;
					broadcast.Serial = user->GetSerial();
					user->SendToAll(broadcast); // broadcasting

					//Logging
					if (user->IsPartOfGuild() == true) {
						GetGuildsUserManager().LeavedUser(user);
					}

					/*if (user->achievement.IsConnected()) {
						Net::Protocol::Achievement::ServerProtocol::GuildActionChanged packet;
						packet.characterName = user->GetName();
						packet.Action = Net::Protocol::Achievement::ServerProtocol::GuildActionChanged::Action::Guild_Leave;
						user->achievement.Send(packet);
					}*/

					std::wstring guildName;
					int guildId = 0;
					//AutoLock lock(user->GetSyncObject());
					if (user->IsPartOfGuild() == true) {
						guildName = user->GetGuildInfo().GuildName;
						guildId = user->GetGuildInfo().GuildId;
					}
					else {
						guildName = L"Unknown";
					}

					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-removed", user->GetName(), Http::Logger::Parameter()
						<< guildId << guildName.c_str());*/

						/* post processing */
					user->ClearGuildInfo();

					user->RemoveGuildAdvantage();

				}
				user->Send(response);
			}

			void GuildManager::MemberListed(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
					return;
				Protocol::FromServer::AllMGuild::ListMembers response;
				if (!answer)
				{
					response.Result = Protocol::FromServer::AllMGuild::ListMembers::Failed;
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::ListMembers::Ok;
					AllMGuildUserInfo member;
					uint16 i = 0;
					InstanceEx::NormalBitfields lastLoginTime(0);
					DateTime now(DateTime::Now());

					for (auto& memb : answer.resultObject) {
						memb.at("characterName").get_to(member.CharacterName);
						member.CharacterInfo = memb.at("characterInfo").get<uint32>();

						// Instance?? ????
						lastLoginTime = 0;
						DateTime time;
						memb.at("lastLogin").get_to(time);
						lastLoginTime.ExpiredYear = time.Get(DateTime::Unit::Year);
						lastLoginTime.ExpiredMonth = time.Get(DateTime::Unit::Month);
						lastLoginTime.ExpiredDay = time.Get(DateTime::Unit::Day);
						lastLoginTime.ExpiredHour = time.Get(DateTime::Unit::Hour);
						lastLoginTime.ExpiredMinute = time.Get(DateTime::Unit::Minute);
						lastLoginTime.ExpiredSecond = time.Get(DateTime::Unit::Second);
						member.LastLogin = lastLoginTime;

						member.CharacterInfo.Job = memb.at("classType").get<uint32>();
						member.CharacterInfo.Level = memb.at("level").get<uint16>();
						member.CharacterInfo.PvPLevel = memb.at("pvpLevel").get<uint16>();

						member.Contributed = memb.at("contribuition").get<uint32>();

						memb.at("privateMessage").get_to(member.PrivateMessage);

						response.Members.push_back(member);
					}
				}

				response.isSeparate = false;

				user->Send(response);
			}

			void GuildManager::NameValidated(const UserSharedPtr& user, Net::Answer& answer)
			{
				Protocol::FromServer::AllMGuild::ValidateName response;
				if (answer)
					response.Result = Protocol::FromServer::AllMGuild::ValidateName::Ok;
				else
					response.Result = Protocol::FromServer::AllMGuild::ValidateName::AlreadyUsed;

				user->Send(response);
			}
			
			void GuildManager::AliasValidated(const UserSharedPtr& user, Net::Answer& answer)
			{
				Protocol::FromServer::AllMGuild::ValidateAlias response;
				if (answer)
					response.Result = Protocol::FromServer::AllMGuild::ValidateAlias::Ok;
				else
					response.Result = Protocol::FromServer::AllMGuild::ValidateAlias::AlreadyUsed;
				user->Send(response);
			}

			void GuildManager::MyInfoRefreshed(const UserSharedPtr& user, Net::Answer& anwser)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
					return;

				Protocol::FromServer::AllMGuild::SetPlayer response;
				if (anwser)
				{
					size_t i = 0;
					GuildInfo& info = response.GuildInfo;

					JoinedInfoParse(anwser, info);
					{
						AutoLock lock(user->GetSyncObject());
						if (user->IsPartOfGuild() == true) {
							info.StartedTimeToPlay = user->GetGuildInfo().StartedTimeToPlay;
							info.MyPlayTime = user->GetGuildInfo().MyPlayTime;
							info.ConnnectionDate = user->GetGuildInfo().ConnnectionDate;
							DateTime startedTimeToPlay(DateTime::Now());
							info.MyPlayTime = Database::Info::GuildInfoManager::CalculatePlayTime(info.StartedTimeToPlay, startedTimeToPlay, info.MyPlayTime);
							info.StartedTimeToPlay = startedTimeToPlay;
						}
						/* post process */
						user->SetGuildInfo(info);
					}
					response.Serial = user->GetSerial();
					user->Send(response);
					GetGuildsUserManager().SetGuildLevel(info.GuildId, info.GuildLevel, info.GuildExp);
				}
			}

			void GuildManager::AddedGuildExp(const UserSharedPtr& user, Net::Answer& anwser)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!anwser) {
					AutoLock lock(user->GetSyncObject());
					LoggerInstance().Error("Guild : DBRequest AddMaintenancePoint Failed User[{}]", user->GetSerial());

					if (user->IsPartOfGuild() == false) {
						return;
					}
					Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint packet;
					packet.Result = Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint::Results::Failed;
					packet.GuildMaintenacePoint = user->GetGuildInfo().Point;
					packet.ShopOpenDate = user->GetGuildInfo().ShopOpenDate;
					packet.ShopCloseDate = user->GetGuildInfo().ShopCloseDate;
					user->Send(packet);
				}
				else
				{
					AutoLock lock(user->GetSyncObject());
					//guild Point???? ó?? ???.			
					if (user->IsPartOfGuild() == false) {
						user->CriticalError("user have not guild info", false);
						return;
					}
					//guild->GuildLevel = static_cast<uint8>( StringUtil::To<int>( (*packet)[0] ) );
					anwser.get_to("Point", user->GetGuildInfo().Point);
					anwser.get_to("shopOpenDate", user->GetGuildInfo().ShopOpenDate);
					anwser.get_to("shopCloseDate", user->GetGuildInfo().ShopCloseDate);

					Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint response;
					response.Result = Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint::Results::Ok;
					response.GuildMaintenacePoint = user->GetGuildInfo().Point;
					response.ShopOpenDate = user->GetGuildInfo().ShopOpenDate;
					response.ShopCloseDate = user->GetGuildInfo().ShopCloseDate;
					anwser.get_to("increasePoint", response.IncreasePoint);
					user->Send(response);

					//logging 
					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-maintain", user->GetName(), Http::Logger::Parameter()
						<< user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildName.c_str() << response.GuildMaintenacePoint << response.ShopOpenDate.ToString() << response.ShopCloseDate.ToString() << response.IncreasePoint);*/
				}

				
			}

			void GuildManager::AddedGuildExpByItem(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
					AutoLock lock(user->GetSyncObject());
					LoggerInstance().Error("Guild : DBRequest AddExp Failed User[{}]", user->GetSerial());

					if (user->IsPartOfGuild() == false) {
						return;
					}
					Protocol::FromServer::AllMGuild::AddedGuildExp packet;
					packet.Result = Protocol::FromServer::AllMGuild::AddedGuildExp::Results::Failed;
					packet.GuildLevel = user->GetGuildInfo().GuildLevel;
					packet.GuildExp = user->GetGuildInfo().GuildExp;
					packet.PlayTime = user->GetGuildInfo().MyPlayTime;
					packet.StartedTimeToPlay = user->GetGuildInfo().StartedTimeToPlay;
					packet.IsByItem = true;
					user->Send(packet);
				}
				else
				{
					uint8 guildLevel = answer.at("level").get<uint8>();
					uint32 guildExp = answer.at("exp").get<uint32>();
					int guildId = 0;
					float myPlayTime = 0.0f;
					DateTime startedTimeToPlay(DateTime::Now());
					{
						AutoLock lock(user->GetSyncObject());
						//guild exp???? ó?? ???.							
						if (user->IsPartOfGuild() == false) {
							user->CriticalError("user have not guild info", false);
							return;
						}
						guildId = user->GetGuildInfo().GuildId;

						uint8 oldGuildLevel = user->GetGuildInfo().GuildLevel;
						float playTime = Database::Info::GuildInfoManager::CalculatePlayTime(user->GetGuildInfo().StartedTimeToPlay, startedTimeToPlay, user->GetGuildInfo().MyPlayTime);

						//debug?? exp ???? factor.
						if (debugExpTimeFactor > 1.0f) {
							float time = 0.0f;
							const static int secondOfDay = 60 * 60 * 24;//(60?? * 60?? * 24?ð?/??? ??? ????? ????).
							time += startedTimeToPlay.GetTime().GetCumulatedSec() - user->GetGuildInfo().StartedTimeToPlay.GetTime().GetCumulatedSec();
							time += (startedTimeToPlay.GetDate().GetCumulatedDay() - user->GetGuildInfo().StartedTimeToPlay.GetDate().GetCumulatedDay()) * secondOfDay;
							time *= debugExpTimeFactor;
							DateTime temp(startedTimeToPlay);
							temp.Add(DateTime::Unit::Second, (uint32)time);
							playTime = Database::Info::GuildInfoManager::CalculatePlayTime(user->GetGuildInfo().StartedTimeToPlay, temp, user->GetGuildInfo().MyPlayTime);
						}
						user->GetGuildInfo().MyPlayTime = myPlayTime = playTime - static_cast<float>(answer.at("requestGuildExp").get<int>() * (Database::DatabaseInstance().InfoCollections.GuildInfos.GetGeneral().MinuteForExp * 60));
						user->GetGuildInfo().StartedTimeToPlay = startedTimeToPlay;
						user->GetGuildInfo().MyContributed += 1;
						user->SendRewardMailByGuildContibutedUp();
					}
					GetGuildsUserManager().SetGuildLevel(guildId, guildLevel, guildExp);

					Protocol::FromServer::AllMGuild::AddedGuildExp response;
					response.Result = Protocol::FromServer::AllMGuild::AddedGuildExp::Results::Ok;
					response.GuildLevel = guildLevel;
					response.GuildExp = guildExp;
					response.PlayTime = myPlayTime;
					response.StartedTimeToPlay = startedTimeToPlay;
					answer.get_to("changedLevel", response.ChangedLevel);
					response.IsByItem = true;
					user->Send(response);

					//logging
					if (response.ChangedLevel == true)
					{
						/*Logger().Write(IAdmLogger::ImportantLogger, "guild-levelup", user->GetName(), Http::Logger::Parameter()
							<< guildId << user->GetGuildInfo().GuildName.c_str() << guildLevel);*/
					}

				}
			}

			void GuildManager::IncresedGuildRankPointByItem(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
					AutoLock lock(user->GetSyncObject());
					LoggerInstance().Error("Guild : DBRequest AddRankingPoint Failed User[{}]", user->GetSerial());

					if (user->IsPartOfGuild() == false) {
						return;
					}
					Protocol::FromServer::AllMGuild::AddedRankingPoint packet;
					packet.Result = Protocol::FromServer::AllMGuild::AddedRankingPoint::Results::Failed;
					user->Send(packet);
				}
				else
				{
					Protocol::FromServer::AllMGuild::AddedRankingPoint response;
					response.Result = Protocol::FromServer::AllMGuild::AddedRankingPoint::Results::Ok;
					answer.get_to("oldRankingPoint", response.OldRankingPoint);
					answer.get_to("newRankingPoint", response.NewRankingPoint);

					user->Send(response);

					//logging
					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-add-guildplaytime", user->GetName(), Http::Logger::Parameter()
						<< user->GetGuildInfo().GuildId << oldGuildRankingPoint << newGuildRankingPoint);*/
				}
			}

			void GuildManager::AddedGuildPoint(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
					AutoLock lock(user->GetSyncObject());
					LoggerInstance().Error(L"Guild : DBRequest AddMaintenancePoint Failed User[{}]", user->GetSerial());

					if (!user->IsPartOfGuild()) {
						return;
					}
					Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint packet;
					packet.Result = Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint::Results::Failed;
					packet.GuildMaintenacePoint = user->GetGuildInfo().Point;
					packet.ShopOpenDate = user->GetGuildInfo().ShopOpenDate;
					packet.ShopCloseDate = user->GetGuildInfo().ShopCloseDate;
					user->Send(packet);
				}
				else
				{
					if (!user->IsPartOfGuild()) {
						user->CriticalError("user have not guild info", false);
						return;
					}
					answer.get_to("Point", user->GetGuildInfo().Point);
					answer.get_to("shopOpenDate", user->GetGuildInfo().ShopOpenDate);
					answer.get_to("shopCloseDate", user->GetGuildInfo().ShopCloseDate);

					Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint response;
					response.Result = Protocol::FromServer::AllMGuild::AddedGuildMaintenancePoint::Results::Ok;
					response.GuildMaintenacePoint = user->GetGuildInfo().Point;
					response.ShopOpenDate = user->GetGuildInfo().ShopOpenDate;
					response.ShopCloseDate = user->GetGuildInfo().ShopCloseDate;
					answer.get_to("increasePoint", response.IncreasePoint);
					user->Send(response);

					//logging 
					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-maintain", user->GetName(), Http::Logger::Parameter()
						<< user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildName.c_str() << response.GuildMaintenacePoint << response.ShopOpenDate.ToString() << response.ShopCloseDate.ToString() << response.IncreasePoint);*/
				}
			}
			
			void GuildManager::AddedItemToGuildShop(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
					user->CriticalError("AddedItemToGuildShop Failed", false);
				}
				else
				{
					size_t i = 0;
					Protocol::FromServer::AllMGuild::AddedGuildShopItem response;
					int guildId = answer.at("id").get<int>();
					answer.get_to("hash", response.Item.ItemHash);
					answer.get_to("expireDate", response.Item.ExpiredDate);
					GetGuildsUserManager().AddGuildShopItem(guildId, response.Item.ItemHash, response.Item.ExpiredDate);


					{
						AutoLock lock(user->GetSyncObject());
						if ((user->IsPartOfGuild() == true) && (user->GetGuildInfo().GuildId == guildId)) {
							user->Send(response);
						}
					}

					//logging
					/*Logger().Write(IAdmLogger::ImportantLogger, "guild-add-product", user->GetName(), Http::Logger::Parameter()
						<< guildId << user->GetGuildInfo().GuildName.c_str() << response.Item.ItemHash);*/
				}
			}

			void GuildManager::ReflashedGuildLevel(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
				}
				else
				{
					int guildId = 0;
					{
						AutoLock lock(user->GetSyncObject());
						if (user->IsPartOfGuild() == false) {
							return;
						}
						guildId = user->GetGuildInfo().GuildId;
					}
					uint8 level = answer.at("level").get<uint8>();
					uint32 exp = answer.at("exp").get<uint32>();
					GetGuildsUserManager().SetGuildLevel(guildId, level, exp);
				}
			}
			
			void GuildManager::ReflashedGuildProfitDate(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {	
				}
				else
				{
					GuildInfo info;
					JoinedInfoParse(answer, info);

					int guildId = info.GuildId;
					DateTime startDate = info.ShopOpenDate;
					DateTime endDate = info.ShopCloseDate;
					GetGuildsUserManager().SetGuildProfitDate(guildId, startDate, endDate);
				}
			}
			
			void GuildManager::ListGuildShopItem(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
					//user->CriticalError("ListGuildShopItem Failed", false);
				}
				else
				{
					int guildId = 0;
					{
						AutoLock lock(user->GetSyncObject());
						if (user->IsPartOfGuild() == false) {
							return;
						}
						guildId = user->GetGuildInfo().GuildId;
					}
					// guildLevel, sub(itemHash, expiredDate)
					Protocol::FromServer::AllMGuild::GuildShopItemList response;
					ListGuildShopItemParse(answer, response.GuildShopItems);

					GetGuildsUserManager().SetGuildShopItem(guildId, response.GuildShopItems);

					user->Send(response);
				}
			}
			
			void GuildManager::ReflashedListGuildShopItem(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
				}
				else
				{
					int guildId = 0;
					{
						AutoLock lock(user->GetSyncObject());
						if (!user->IsPartOfGuild()) {
							return;
						}
						guildId = user->GetGuildInfo().GuildId;
					}
					uint8 guildLevel = 0;
					Protocol::FromServer::AllMGuild::GuildShopItemList response;
					ListGuildShopItemParse(answer, response.GuildShopItems);
					GetGuildsUserManager().SetGuildShopItem(guildId, response.GuildShopItems);
				}
			}

			void GuildManager::GuildRankingListed(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
					user->CriticalError("ListGuildRanking Failed", false);
				}
				else
				{
					AutoLock lock(user->GetSyncObject());
					//if( user->IsPartOfGuild() == false ) {			/// by kpongky( 09.06.29 ) remove this code to send guild rank list for users who didn't join any guild.
					//	return;
					//}
					GuildRankInfo guildRank;
					uint16 i = 0;

					TimeSpan now;
					now.SetTimeSpan(DateTime::Now());
					TimeSpan expireDate;

					Protocol::FromServer::AllMGuild::GuildRankList	response;
					response.Result = Protocol::FromServer::AllMGuild::GuildRankList::Results::Ok;

					for(auto& rank : answer.at("ranks")){
						expireDate.SetTimeSpan(rank.at("expireDate").get<DateTime>());
						if (expireDate < now)
						{
							response.RankList.clear();
							break;
						}
						rank.at("rank").get_to(guildRank.Rank);
						rank.at("totalPlayTime").get_to(guildRank.TotalPlayTime);
						rank.at("id").get_to(guildRank.GuildID);
						if (guildRank.GuildID == user->GetGuildInfo().GuildId)
						{
							user->GetGuildInfo().Rank = guildRank.Rank;
						}

						rank.at("guildName").get_to(guildRank.GuildName);
						rank.at("guildMemberCount").get_to(guildRank.GuildMemberCount);
						rank.at("guildLevel").get_to(guildRank.GuildLevel);
						rank.at("currentTotalPlayTime").get_to(guildRank.CurrentTotalPlayTime);

						response.RankList.push_back(guildRank);
					}
					user->Send(response);
				}
			}
			
			void GuildManager::TaxGained(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!answer) {
					user->CriticalError("TaxGained Failed", false);
				}
				else
				{
					AutoLock lock(user->GetSyncObject());

					if (user->IsPartOfGuild() == false || user->GetGuildInfo().GuildId != answer.at("id").get<int>())
					{
						return;
					}
					Protocol::FromServer::AllMGuild::TaxGained	response;
					response.Result = Protocol::FromServer::AllMGuild::TaxGained::Results::Ok;
					answer.get_to("tax", response.Tax);
					answer.get_to("taxPayDate", response.TaxPayDate);

					user->Send(response);

					uint32 gold = 0;
					answer.at("amount").get<uint32>();
					user->SetMoney(user->GetMoney() + gold);
					Protocol::FromServer::GoldGain packet;
					packet.serial = user->GetSerial();
					packet.money = gold;
					user->Send(packet);

					////loggin
					//Logger().Write(IAdmLogger::ImportantLogger, "guild-get-tax", user->GetName(), Http::Logger::Parameter()
					//	<< user->GetGuildInfo().GuildId << user->GetGuildInfo().GuildName.c_str() << gold);
				}
			}

			void GuildManager::SetPlayer(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Protocol::FromServer::AllMGuild::SetPlayer response;
				//DB?? ??û??? GuildInfo?? ????.
				user->SetRequestedInitGuildInfo(false);
				if (!answer)
				{
					// nothing to do			
				}
				else
				{
					//' response : guild Info (separated : guildId,guildName,guildAlias,message,masterName,memberCount,grade,guildMemberId,guildLevel,guildExp,contributed,playTime,point,shopStart,shopEnd
					//									gradeName,authority, [gradeName, authority, gradeName, authority...] )
					//' remarks  ; no error for not joined user to avoid bulky error message
					
					// I honestly don't know what this is //TELES
					//if (!IsJoinedUserPacket(answer))
					//{
					//	if (user->IsPartOfGuild() == true)
					//	{
					//		// Guild User Manager???? ????? ???? ?????? ??????.					
					//		LoggerInstance().Error(L"ClearGuildInfo[{}] From SetPlayer", user->GetName().c_str());
					//		GetGuildsUserManager().LeavedUser(user);
					//		user->ClearGuildInfo();
					//	}

					//	RequestGuildRankList(user);

					//	UserManagerInstance().SendCurrentStageEvents(user);
					//	return; // not joined user
					//}

					size_t i = 0;
					GuildInfo& info = response.GuildInfo;
					JoinedInfoParse(answer, info);
					response.Serial = user->GetSerial();

					if (user->IsPartOfGuild() == false) {
						user->SetGuildInfo(info);
						GetGuildsUserManager().JoinedUser(user);
					}
					else {
						info.StartedTimeToPlay = user->GetGuildInfo().StartedTimeToPlay;
						info.MyPlayTime = user->GetGuildInfo().MyPlayTime;
						info.ConnnectionDate = user->GetGuildInfo().ConnnectionDate;
						DateTime startedTimeToPlay(DateTime::Now());
						info.MyPlayTime = Database::Info::GuildInfoManager::CalculatePlayTime(info.StartedTimeToPlay, startedTimeToPlay, info.MyPlayTime);
						info.StartedTimeToPlay = startedTimeToPlay;
						user->SetGuildInfo(info);

					}
					user->SendToAll(response);
					//??? ?????? ?????? ?????? ??????? ??û???.
					Net::Api request("Guild_v3.ListGuildShopProduct");
					request << info.GuildId;
					request.GetAsync(this, &GuildManager::ListGuildShopItem, user);
					GetGuildsUserManager().SetGuildLevel(info.GuildId, info.GuildLevel, info.GuildExp);

					RequestGuildRankList(user);

					UserManagerInstance().SendCurrentStageEvents(user);
				}
			}

			void GuildManager::MyInfoVaild(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Protocol::FromServer::AllMGuild::SetPlayer response;
				if (!answer)
				{
					// nothing to do
				}
				else
				{
					// guildAlias use to guild v3. 2008-07-30 by kwind.
					//' response : guild Info (separated : guildId, guildName, guildAlias, message, masterName, memberCount, myGrade, myGuildMemberId, [gradeName, authority, gradeName, authority...] )
					//' remarks  ; no error for not joined user to avoid bulky error message

					size_t i = 0;
					GuildInfo info;
					JoinedInfoParse(answer, info);
					{
						AutoLock lock(user->GetSyncObject());
						if (user->IsPartOfGuild() == true) {
							info.StartedTimeToPlay = user->GetGuildInfo().StartedTimeToPlay;
							info.MyPlayTime = user->GetGuildInfo().MyPlayTime;
							info.ConnnectionDate = user->GetGuildInfo().ConnnectionDate;
							DateTime startedTimeToPlay(DateTime::Now());
							info.MyPlayTime = Database::Info::GuildInfoManager::CalculatePlayTime(info.StartedTimeToPlay, startedTimeToPlay, info.MyPlayTime);
							info.StartedTimeToPlay = startedTimeToPlay;
						}

						/* post process */
						user->SetGuildInfo(info);
					}

					GetGuildsUserManager().SetGuildLevel(info.GuildId, info.GuildLevel, info.GuildExp);

					if (user->ValidGuildInfoAndRun(info) == false) {
						return;
					}
				}
			}

			void GuildManager::MessageChanged(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (user->IsPartOfGuild() == false) return; // temporary. i hate bulky NULL pointer validation =(
				Protocol::FromServer::AllMGuild::MessageChange response;

				if (!answer)
				{
					response.Result = static_cast<Protocol::FromServer::AllMGuild::MessageChange::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::MessageChange::Ok;

					AutoLock lock(user->GetSyncObject());
					answer.get_to("message", user->GetGuildInfo().Message);
				}
				user->Send(response);
			}

			void GuildManager::GradeChanged(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (user->IsPartOfGuild() == false) return; // temporary. i hate bulky NULL pointer validation =(

				Protocol::FromServer::AllMGuild::ChangeGrade response;
				if (!answer)
				{
					response.Result = static_cast<Protocol::FromServer::AllMGuild::ChangeGrade::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::ChangeGrade::Ok;
					answer.get_to("characterName", response.TargetCharacterName);
					answer.get_to("grade", response.Grade);
				}
				user->Send(response);
			}

			void GuildManager::GradeNameChanged(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (!user->IsPartOfGuild()) return; // temporary. i hate bulky NULL pointer validation =(

				Protocol::FromServer::AllMGuild::ChangeGradeName response;
				if (!answer)
				{
					response.Result = static_cast<Protocol::FromServer::AllMGuild::ChangeGradeName::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::ChangeGradeName::Ok;
					answer.get_to("targetGrade", response.TargetGrade);
					answer.get_to("TargetGradeName", response.TargetGradeName);
				}
				user->Send(response);
			}

			void GuildManager::GradeAuthChanged(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (user->IsPartOfGuild() == false) return; // temporary. i hate bulky NULL pointer validation =(

				Protocol::FromServer::AllMGuild::ChangeGradeAuth response;
				if (!answer)
				{
					response.Result = static_cast<Protocol::FromServer::AllMGuild::ChangeGradeAuth::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::ChangeGradeAuth::Ok;
					answer.get_to("targetGrade", response.TargetGrade);
					answer.get_to("authority", response.Authority);
				}
				user->Send(response);
			}

			void GuildManager::GuildMasterChanged(const UserSharedPtr& user, Net::Answer& answer)
			{
				if (user && user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (user->IsPartOfGuild() == false) return; // temporary. i hate bulky NULL pointer validation =(

				Protocol::FromServer::AllMGuild::ChangeGuildMaster response;
				if (!answer)
				{
					response.Result = static_cast<Protocol::FromServer::AllMGuild::ChangeGuildMaster::Results>(answer.errorCode);
				}
				else
				{
					response.Result = Protocol::FromServer::AllMGuild::ChangeGuildMaster::Ok;
					answer.get_to("characterName", response.TargetCharacterName);

					{
						AutoLock lock(user->GetSyncObject());
						user->GetGuildInfo().MasterName = response.TargetCharacterName;
						user->GetGuildInfo().MyGrade = 1; //downgrade my grade.
					}

					auto target = UserManagerInstance().GetUserByName(response.TargetCharacterName);
					if (target) {
						AutoLock lock(target->GetSyncObject());
						if (target->IsPartOfGuild() == true) {
							target->GetGuildInfo().MasterName = response.TargetCharacterName;
							target->GetGuildInfo().MyGrade = 0; //upgrade target's grade.
						}
					}
				}
				user->Send(response);
			}
			
			void GuildManager::RequestReflashGuildLevel(UserSharedPtr user, int guildId, uint8 guildLevel)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (GetGuildsUserManager().TryReflashGuildLevel(guildId, guildLevel) == true) {
					Net::Api request("Guild_v3.CheckLevel");
					request << guildId << guildLevel;
					request.GetAsync(this, &GuildManager::ReflashedGuildLevel, user);
				}
			}
			
			void GuildManager::JoinedInfoParse(const Net::Answer& answer, XRated::AllMGuildInfo& info)
			{
				size_t i = 0;
				answer.get_to("guildId", info.GuildId);
				answer.get_to("guildName", info.GuildName);
				answer.get_to("guildAlias", info.GuildAlias);
				answer.get_to("message", info.Message);
				answer.get_to("memberCount", info.MemberCount);
				answer.get_to("guildLevel", info.GuildLevel);
				answer.get_to("guildExp", info.GuildExp);
				answer.get_to("totalPlayTime", info.TotalPlayTime);
				answer.get_to("tax", info.Tax);
				answer.get_to("taxPayDay", info.TaxPayDay);
				answer.get_to("guildMemberId", info.GuildMemberId);
				answer.get_to("myGrade", info.MyGrade);
				answer.get_to("myContributed", info.MyContributed);
				answer.get_to("myPlayTime", info.MyPlayTime);
				answer.get_to("point", info.Point);
				answer.get_to("shopOpenDate", info.ShopOpenDate);
				answer.get_to("shopCloseDate", info.ShopCloseDate);
				answer.get_to("rank", info.Rank);
				answer.get_to("rankExpiredDate", info.RankExpiredDate);
				info.ConnnectionDate = DateTime::Now();
				info.StartedTimeToPlay = info.ConnnectionDate;

				std::pair<bool, uint32> nextLevelExp = Database::DatabaseInstance().InfoCollections.GuildInfos.GetNextLevelExp(info.GuildLevel);
				if (nextLevelExp.first == false) {
					info.MyPlayTime = 0.0f;
				}
				for (auto& gInfo : answer.at("gradeInfo")) {
					info.Grade.push_back(GuildInfo::GradeInfo(
						gInfo.at("name").get<std::wstring>(), 
						gInfo.at("authority").get<uint8>()
					));
				}

			}
			
			void GuildManager::ListGuildShopItemParse(const Net::Answer& answer, std::vector<XRated::GuildShopItem>& guildItems)
			{
				// guildLevel, sub(itemHash, expiredDate)
				size_t i = 0;
				// sub : itermHash,expireDate
				for(auto& item : answer.at("items")){
					guildItems.push_back(GuildShopItem(
						item.at("id").get<uint32>(), 
						item.at("expireDate").get<DateTime>()
						));
				}
			}

			GuildsUserManager::GuildUserManager::GuildUserManager() : lastDbRequestedGuildLevel(0)
			{
				lastDbRequestedGuildShopItem.ItemHash = 0;
			}
		
			GuildsUserManager::GuildUserManager::~GuildUserManager()
			{
			}

			void GuildsUserManager::GuildUserManager::Add(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				if (IsUser(user) == false) {
					uint8 guildLevel = 0;
					uint32 guildExp = 0;
					{
						AutoLock lock(user->GetSyncObject());
						if (user->IsPartOfGuild() == false) {
							return;
						}
						guildLevel = user->GetGuildInfo().GuildLevel;
						guildExp = user->GetGuildInfo().GuildExp;
						info = (user->GetGuildInfo());
					}
					connectedMemberList.push_back(user);	
					if (info.GuildLevel < guildLevel) {
						SetGuildLevel(guildLevel, guildExp);
					}
				}
				else {
					LoggerInstance().Error("GuildUserManager::Add, Exist User : {}", user->GetSerial());
				}
			}
		
			void GuildsUserManager::GuildUserManager::Remove(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				ConnectedMemberList::iterator iter = std::find_if(connectedMemberList.begin(), connectedMemberList.end(), UserFinder(user));
				if (iter != connectedMemberList.end()) {
					connectedMemberList.erase(iter);
				}
				else {
					LoggerInstance().Warn("GuildUserManager::Remove, Not Exist User : {}", user->GetSerial());
				}
			}

			void GuildsUserManager::GuildUserManager::SetGuildLevel(uint8 guildLevel, uint32 guildExp)
			{
				info.GuildLevel = guildLevel;
				info.GuildExp = guildExp;
				ConnectedMemberList::iterator iter = connectedMemberList.begin();
				ConnectedMemberList::iterator end = connectedMemberList.end();
				while (iter != end) {
					(*iter)->ChangedGuildLevel(guildLevel, guildExp);
					++iter;
				}
			}
			
			void GuildsUserManager::GuildUserManager::SetGuildProfitDate(DateTime startDate, DateTime endDate)
			{
				info.ProfitStartDate = startDate;
				info.ProfitEndDate = endDate;
				ConnectedMemberList::iterator iter = connectedMemberList.begin();
				ConnectedMemberList::iterator end = connectedMemberList.end();
				while (iter != end) {
					(*iter)->ChangedGuildProfitDate(startDate, endDate);
					++iter;
				}
			}
			
			void GuildsUserManager::GuildUserManager::AddGuildShopItem(uint32 itemHash, DateTime expiredDate)
			{
				GuildShopItemList::iterator iter = std::find_if(guildShopItems.begin(), guildShopItems.end(), GuildShopItem::FindByHash(itemHash));
				if (iter != guildShopItems.end()) {
					(*iter).ExpiredDate = expiredDate;
				}
				else {
					guildShopItems.push_back(GuildShopItem(itemHash, expiredDate));
				}
			}
			
			void GuildsUserManager::GuildUserManager::SetGuildShopItem(const std::vector<XRated::GuildShopItem>& items)
			{
				//?????? ?????? ???? ???????.
				guildShopItems = items;
			}

			bool GuildsUserManager::GuildUserManager::IsUser(UserSharedPtr user) const
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return false;
				}

				ConnectedMemberList::const_iterator iter = std::find_if(connectedMemberList.begin(), connectedMemberList.end(), UserFinder(user));
				if (iter != connectedMemberList.end()) {
					return true;
				}
				return false;
			}
		
			bool GuildsUserManager::GuildUserManager::IsEmpty() const
			{
				return connectedMemberList.empty();
			}
			
			void GuildsUserManager::GuildUserManager::SetLastRequestedGuildLevel(uint8 level)
			{
				lastDbRequestedGuildLevel = level;
			}
			
			uint8 GuildsUserManager::GuildUserManager::GetLastRequestedGuildLevel() const
			{
				return lastDbRequestedGuildLevel;
			}
		
			uint8 GuildsUserManager::GuildUserManager::GetCurrentGuildLevel() const
			{
				return info.GuildLevel;
			}
			//
			bool GuildsUserManager::GuildUserManager::IsExistGuildShopItem(uint32 itemHash) const
			{
				GuildShopItemList::const_iterator iter = std::find_if(guildShopItems.begin(), guildShopItems.end(), GuildShopItem::FindByHash(itemHash));
				if (iter != guildShopItems.end()) {
					DateTime now(DateTime::Now());
					if (now < (*iter).ExpiredDate) {
						return true;
					}
					else {
						//1???????? ??????.
						DateTime temp((*iter).ExpiredDate);
						temp.Add(DateTime::Unit::Minute, 1);
						if (now < temp) {
							return true;
						}
					}
				}
				return false;
			}
			
			bool GuildsUserManager::GuildUserManager::IsExistGuildShopItem(uint32 itemHash, DateTime expiredDate) const
			{
				GuildShopItemList::const_iterator iter = std::find_if(guildShopItems.begin(), guildShopItems.end(), GuildShopItem::FindByHash(itemHash));
				if (iter != guildShopItems.end()) {
					if (expiredDate == (*iter).ExpiredDate) {
						return true;
					}
					else if (expiredDate == lastDbRequestedGuildShopItem.ExpiredDate) {
						return true;
					}
				}
				return false;
			}
			
			bool GuildsUserManager::GuildUserManager::SetLastDbRequestedGuildShopItem(const XRated::GuildShopItem& item)
			{
				if ((lastDbRequestedGuildShopItem.ItemHash != item.ItemHash)) {
					lastDbRequestedGuildShopItem = item;
					return true;
				}
				if ((lastDbRequestedGuildShopItem.ExpiredDate != item.ExpiredDate)) {
					lastDbRequestedGuildShopItem = item;
					return true;
				}
				return false;
			}
			
			bool GuildsUserManager::GuildUserManager::SetLastDbRequestedGuildProfitDate(DateTime start, DateTime end)
			{
				if ((lastDbRequestedProfitDate.first != start) || (lastDbRequestedProfitDate.second < end)) {
					lastDbRequestedProfitDate.first = start;
					lastDbRequestedProfitDate.second = end;
					return true;
				}
				return false;
			}
			
			void GuildsUserManager::AddGuildShopItem(int guildId, uint32 itemHash, DateTime expiredDate)
			{
				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					iter->second.AddGuildShopItem(itemHash, expiredDate);
				}
			}
			
			void GuildsUserManager::SetGuildShopItem(int guildId, const std::vector<XRated::GuildShopItem>& items)
			{
				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					iter->second.SetGuildShopItem(items);
				}
			}

			void GuildsUserManager::SetGuildLevel(int guildId, uint8 guildLevel, uint32 guildExp)
			{
				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					iter->second.SetGuildLevel(guildLevel, guildExp);
				}
			}
			
			void GuildsUserManager::SetGuildProfitDate(int guildId, DateTime startDate, DateTime endDate)
			{
				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					iter->second.SetGuildProfitDate(startDate, endDate);
				}
			}
		
			GuildsUserManager::GuildsUserManager()
			{
			}

			GuildsUserManager::~GuildsUserManager()
			{
			}

			GuildsUserManager& GuildsUserManager::GetInstance()
			{
				static GuildsUserManager m_GuildsUserManagerInstance;
				return m_GuildsUserManagerInstance;
			}
			
			void GuildsUserManager::JoinedUser(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				int guildId = 0;
				{
					AutoLock lock(user->GetSyncObject());
					if (user->IsPartOfGuild() == false) {
						LoggerInstance().Warn("Guild User Manager Joined Failed, this user is not joined guild : {}", user->GetSerial());
						return;
					}
					guildId = user->GetGuildInfo().GuildId;
				}

				GuildUserManager* guildUserManager = NULL;

				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					guildUserManager = &(iter->second);
				}
				else {
					guildUserManager = &(guildTable[guildId]);
				}
				guildUserManager->Add(user);
			}
			
			void GuildsUserManager::LeavedUser(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				int guildId = 0;
				{
					AutoLock lock(user->GetSyncObject());
					if (user->IsPartOfGuild() == false) {
						LoggerInstance().Warn("Guild User Manager Leaved Failed, this user is not joined guild : {}", user->GetSerial());
						return;
					}
					guildId = user->GetGuildInfo().GuildId;
				}

				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					iter->second.Remove(user);
					//??? guild?? ????? user?? ????? ??? ??????? ???? ???.
					if (iter->second.IsEmpty() == true) {
						guildTable.erase(iter);
					}
				}

			}
		
			bool GuildsUserManager::TryReflashGuildLevel(int guildId, uint8 level)
			{
				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					if ((iter->second.GetCurrentGuildLevel() < level) && (iter->second.GetLastRequestedGuildLevel() < level)) {
						iter->second.SetLastRequestedGuildLevel(level);
						return true;
					}
				}
				return false;
			}
	
			bool GuildsUserManager::IsExistGuildShopItem(int guildId, uint32 itemHash)
			{
				AutoLock lock(cs);
				GuildTable::const_iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					return iter->second.IsExistGuildShopItem(itemHash);
				}
				return false;
			}
		
			bool GuildsUserManager::IsExistGuildShopItem(int guildId, uint32 itemHash, DateTime expiredDate)
			{
				AutoLock lock(cs);
				GuildTable::const_iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					return iter->second.IsExistGuildShopItem(itemHash, expiredDate);
				}
				return false;
			}
			
			bool GuildsUserManager::SetLastDbRequestedGuildShopItem(int guildId, const XRated::GuildShopItem& item)
			{
				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					return iter->second.SetLastDbRequestedGuildShopItem(item);
				}
				return true;
			}
			
			bool GuildsUserManager::SetLastDbRequestedGuildProfitDate(int guildId, DateTime start, DateTime end)
			{
				AutoLock lock(cs);
				GuildTable::iterator iter = guildTable.find(guildId);
				if (iter != guildTable.end()) {
					return iter->second.SetLastDbRequestedGuildProfitDate(start, end);
				}
				return true;
			}

			GuildsUserManager& GetGuildsUserManager()
			{
				return GuildsUserManager::GetInstance();
			}
		}
	}
}
