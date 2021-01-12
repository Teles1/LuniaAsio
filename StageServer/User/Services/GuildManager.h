#pragma once
#include <StageServer/Protocol/ToServer.h>
#include <StageServer/Common.h>
#include <Network/Api/Api.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			/*template<int GuildType>
			class GuildManager
			{
			protected:
				GuildManager() {};
			};
			const static int ALLMGUILDTYPE = Constants::LocaleSpecificSetting::GuildType::AllMGuild;
			*/
			class GuildsUserManager
			{
				class GuildUserManager {
					typedef std::vector<UserSharedPtr >	ConnectedMemberList;
					ConnectedMemberList				connectedMemberList;

					class UserFinder
					{
						UserSharedPtr user;
					public:
						UserFinder(UserSharedPtr in) :user(in) {}
						bool operator()(UserSharedPtr in) const {
							if (user == in) {
								return true;
							}
							return false;
						}
					};
					std::mutex cs;



					typedef std::vector<GuildShopItem>	GuildShopItemList;
					GuildShopItemList					guildShopItems;
				public:
					std::mutex& GetSyncObject() { return cs; }
				private:
					Common::GuildInfo	info;
					uint8 lastDbRequestedGuildLevel;
					XRated::GuildShopItem lastDbRequestedGuildShopItem;
					std::pair<DateTime, DateTime>	lastDbRequestedProfitDate;
				public:
					void SetLastRequestedGuildLevel(uint8 level);
					uint8 GetLastRequestedGuildLevel() const;
					uint8 GetCurrentGuildLevel() const;
					void Add(UserSharedPtr user);
					void Remove(UserSharedPtr user);
					bool IsUser(UserSharedPtr user) const;
					bool IsEmpty() const;
					//void UpdateGuild(const XRated::AllMGuildInfo& guildInfo);
					void SetGuildLevel(uint8 guildLevel, uint32 guildExp);
					void SetGuildProfitDate(DateTime startDate, DateTime endDate);
					void AddGuildShopItem(uint32 itemHash, DateTime expiredDate);
					void SetGuildShopItem(const std::vector<XRated::GuildShopItem>& items);
					bool IsExistGuildShopItem(uint32 itemHash) const;
					bool IsExistGuildShopItem(uint32 itemHash, DateTime expiredDate) const;
					bool SetLastDbRequestedGuildShopItem(const XRated::GuildShopItem& item);
					bool SetLastDbRequestedGuildProfitDate(DateTime start, DateTime end);
				public:
					GuildUserManager();
					virtual ~GuildUserManager();

				};


				std::mutex cs;
				typedef std::map<int, GuildUserManager>	GuildTable;
				GuildTable								guildTable;
			public:
				void SetGuildLevel(int guildId, uint8 guildLevel, uint32 guildExp);
				void SetGuildProfitDate(int guildId, DateTime startDate, DateTime endDate);
				void JoinedUser(UserSharedPtr user);
				void LeavedUser(UserSharedPtr user);
				bool TryReflashGuildLevel(int guildId, uint8 level);
				bool IsExistGuildShopItem(int guildId, uint32 itemHash);
				void AddGuildShopItem(int guildId, uint32 itemHash, DateTime expiredDate);
				void SetGuildShopItem(int guildId, const std::vector<XRated::GuildShopItem>& items);

				bool IsExistGuildShopItem(int guildId, uint32 itemHash, DateTime expiredDate);
				bool SetLastDbRequestedGuildShopItem(int guildId, const XRated::GuildShopItem& item);
				/** SetLastDbRequestedGuildProfitDate.
					@return true:changed, false:same data
				*/
				bool SetLastDbRequestedGuildProfitDate(int guildId, DateTime start, DateTime end);

				GuildsUserManager();
				virtual ~GuildsUserManager();
				static GuildsUserManager& GetInstance();
			};
			GuildsUserManager& GetGuildsUserManager();

			//template<>
			class GuildManager/*<Constants::LocaleSpecificSetting::GuildType::AllMGuild>*/
			{
				float debugExpTimeFactor;
			public:
				GuildManager();
				typedef AllMGuildInfo GuildInfo;
			public: // packet processors
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::Create& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::Invite& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::InviteReply& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::Join& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::Kick& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::Leave& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ListMembers& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::Remove& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ValidateName& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ValidateAlias& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::AddExp& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::AddMaintenancePoint& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ReflashGuildLevel& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::GetTax& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ReflashGuildShopItem& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::MessageChange& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGrade& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGradeName& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGradeAuth& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ChangeGuildMaster& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::GetMyInfo& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::RefreshMyInfo& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AllMGuild::ReflashGuildProfitDate& packet);
			public:
				void RequestGuildInfo(UserSharedPtr user);
				void RequestGuildInfoForJoinUser(UserSharedPtr user);

				void RequestValidGuildInfo(UserSharedPtr user);
				void GuildPointUp(UserSharedPtr user, uint32 point);
				void GuildExpUp(UserSharedPtr user, uint32 exp);
				void IncreaseGuildRankingPoint(UserSharedPtr user, uint32 hours);
				void RequestAddItemToGuildShop(UserSharedPtr user, uint32 addItemHash, uint8 usableGuildLevel, int day);
				void RequestReflashGuildShopItemList(UserSharedPtr user);
				void RequestGuildRankList(UserSharedPtr user);

				void InitCumulativeTax();
				void SaveTax(float tax);
				void UpdateTaxToDB(UserSharedPtr user);
				void TaxUpdated(const UserSharedPtr& user, const Net::Answer& answer);

			private:
				void RequestGuildInfo(UserSharedPtr user, uint32 guildOid);
				void RequestReflashGuildLevel(UserSharedPtr user, int guildId, uint8 level);
				void JoinedInfoParse(const Net::Answer& answer, XRated::AllMGuildInfo& infoOut);
				void ListGuildShopItemParse(const Net::Answer& answer, std::vector<XRated::GuildShopItem>& guildItems);

			private: // db response
				void Created(const UserSharedPtr& user, const Net::Answer& answer);
				void Joined(const UserSharedPtr& user, const Net::Answer& answer);
				void Left(const UserSharedPtr& user, const Net::Answer& answer);
				void Kicked(const UserSharedPtr& user, const Net::Answer& answer);
				void Removed(const UserSharedPtr& user, const Net::Answer& answer);
				void MemberListed(const UserSharedPtr& user, const Net::Answer& answer);
				void NameValidated(const UserSharedPtr& user, const Net::Answer& answer);
				void AliasValidated(const UserSharedPtr& user, const Net::Answer& answer);
				void AddedGuildPoint(const UserSharedPtr& user, const Net::Answer& answer);
				void ReflashedGuildLevel(const UserSharedPtr& user, const Net::Answer& answer);
				void GuildRankingListed(const UserSharedPtr& user, const Net::Answer& answer);
				void TaxGained(const UserSharedPtr& user, const Net::Answer& answer);
				void ReflashedListGuildShopItem(const UserSharedPtr& user, const Net::Answer& answer);
				void SetPlayer(const UserSharedPtr& user, const Net::Answer& answer);
				void MessageChanged(const UserSharedPtr& user, const Net::Answer& answer);
				void GradeChanged(const UserSharedPtr& user, const Net::Answer& answer);
				void GradeNameChanged(const UserSharedPtr& user, const Net::Answer& answer);
				void GradeAuthChanged(const UserSharedPtr& user, const Net::Answer& answer);
				void GuildMasterChanged(const UserSharedPtr& user, const Net::Answer& answer);
				void MyInfoVaild(const UserSharedPtr& user, const Net::Answer& answer);
				void ReflashedGuildProfitDate(const UserSharedPtr& user, const Net::Answer& answer);
				void AddedGuildExpByItem(const UserSharedPtr& user, const Net::Answer& answer);
				void MyInfoRefreshed(const UserSharedPtr& user, const Net::Answer& answer);
				void AddedGuildExp(const UserSharedPtr& user, const Net::Answer& answer);
				void IncresedGuildRankPointByItem(const UserSharedPtr& user, const Net::Answer& answer);
				void AddedItemToGuildShop(const UserSharedPtr& user, const Net::Answer& answer);
				void ListGuildShopItem(const UserSharedPtr& user, const Net::Answer& answer);
			private:

			private:
				const int CreationFee;
				const int MinimalCreationLevel;
				float CumulativeTax;
				std::mutex cs;
			};

		}
	}
}