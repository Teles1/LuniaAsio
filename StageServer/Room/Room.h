#pragma once
#include <Logic/Logic.h>
#include <Logic/Object/Player.h>
#include <StageServer/Room/IUpdateRoom.h>
#include <StageServer/Common.h>
#include <StageServer/User/RoomUserManager.h>
#include <StageServer/Room/Services/Guild.h>
#include <StageServer/User/IUserRoom.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer{
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class Room
				: public IUpdateRoom
				, public Logic::ILogic::IEventListener
				, public IUserRoom
				, public std::enable_shared_from_this<Room>

			{
			public:
				Room(const uint16& index);
			public: //IUpdateRoom
				uint16 GetIndex();
				uint16 GetThreadIndex();
				void SetThreadIndex(const uint16& i);

				bool Update(const float& dt);
				void UpdateExpFactor();
				void NoticeHolidayEvent(const uint32& eventId, bool start);

				bool JoinUser(UserSharedPtr user, const std::string& roomPass);
				bool PartUser(UserSharedPtr user);

				const Logic::ILogic::UpdateInfo& GetLogicUpdateInfo();

				void CheckLoadingTime(const float& dt);
				Constants::GameTypes GetRoomType() const;

				uint16 UserCount() const;
				void Clear();
			public: // IEventListener implementation which sends information from the Logic.
				void Initialized(Database::Info::StageInfo* info, uint16 uniqueId);
				std::mutex& GetSyncRoom();
				void NonPlayerCreated(NonPlayerData& data);
				bool Tamed(Logic::Player* player, Serial familiar, Constants::Familiar::Type type);
				void StructureCreated(CharacterData& data);
				void VehicleCreated(CharacterData& data);
				void ProjectileCreated(ObjectData& obj, const float3& targetPos, Serial target, const String& owner);
				void ItemCreated(ObjectData& data, const String& owner, float dt, uint16 stackCount, bool isPrivateItem);
				void ObjectCreated(ObjectData& data);
				void ObjectDestroyed(Serial gameObjectSerial, Constants::ObjectType type, uint32 hash, bool silent, uint8 team = 255, NonPlayerData::NpcType npcType = NonPlayerData::NpcType::Normal);

				void Died(Logic::Player* player);
				void Revived(Logic::Player* player, uint16 totalReviveCount);
				void InstantCoinRevived(Logic::Player* player, Logic::ILogic::InstantCoinReviveError error, uint64 orderNumber);

				bool IsPvPRoom();

				void AnimationChanged(Serial gameObjectSerial, uint32 animation, const float3& position, const float3& direction, const float param);
				void CollisionStateChanged(Serial gameObjectSerial, bool collision, const float3& position);

				void XpGained(Logic::Player* player, XRated::Constants::ExpAcquiredType type, uint64 storyXp, int32 pvpXp, int32 warXp, Serial beKilledNpc);
				void GoldGained(Logic::Player* player, XRated::Constants::GoldRewardAcquiredType type, uint32 money);
				bool AcquireItem(Logic::Player* player, uint32 itemId, Serial itemSerial, uint16 stackCount, InstanceEx instance);

				void SendPvPRewardItemInfo(Database::Info::PvPRewardInfo rewardInfo);
				void AcquireItemWithMailSupport(Logic::Player* player, uint32 itemId, uint16 stackCount);

				// Async Events for player commands
				bool PlayerCreated(Logic::Player* player);
				void JoinFailed(std::shared_ptr<void> user);
				void ItemUsed(uint32 returnValue, Logic::Player* player, const Database::Info::ItemInfo* info, int bag, int pos);
				void Casted(Serial gameObjectSerial, uint32 skillGroupName, uint8 skilllevel);
				void ItemEquipped(bool bequipped, Logic::Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId);
				void EquipementSwapped(bool bequipped, Logic::Player* player, const std::vector< EquippedItem>& newEquipments);
				void CashEquipementSwapped(bool bequipped, Logic::Player* player, const std::vector< EquippedItem>& newEquipments);
				void ItemEquipToEquipChanged(bool bChanged, Logic::Player* player, Constants::Equipment from, Constants::Equipment to);
				//void SkillPointIncreased(bool bIncreased, Player* player, uint32 skillGroup, int level)=0; ///< @remarks skillGroup parameter can be 0 as all skills
				void SkillPointChanged(Logic::Player* player, uint32 skillGroupHash, int level); ///< skillGroupHash parameter can be 0 as all skills. and level can be less than 0 by setting failure


				// object state changed
				void LevelUp(Logic::Player* player, XRated::Constants::ExpType expType, uint16 level, uint32 storedLevel);
				void SkillPointGot(Logic::Player* player, int skillPoint);
				void StateChanged(Serial gameObjectSerial, uint32 id, int level, Serial byWhom);
				void StatusChanged(Serial gameObjectSerial, float3 pos, float3 dir, float hp, float mp, Serial byWhom, uint32 byWhat, uint32 sFlag, uint32 airComboCount);

				bool AddItem(Logic::Player* player, uint32 id, Serial gameObjectSerial, int cnt);
				bool AddItem(Logic::Player* player, Serial gameObjectSerial, int* arrayId, int* arrayCnt);
				bool RemoveItem(Logic::Player* player, uint32 id, int cnt);
				bool RemoveItem(Logic::Player* player, int* arrayId, int* arrayCnt);
				bool ExamineItemSlotFromPlayer(Logic::Player* player, Database::Info::ItemInfo* info, int cnt);

				void AcquirePvpItem(Logic::Player* player, uint32 itemHash);
				void UsePvpItem(Logic::Player* player, uint32 itemHash);
				void DropPvpItem(Logic::Player* player);

				//Quest
				uint8 GetQuestState(Logic::Player* player, uint32 questHash);
				uint8 SetQuestState(Logic::Player* player, uint32 questHash, uint8 newState);
				uint32 GetQuestParameter(Logic::Player* player, uint32 questHash, uint8 paramIndex);
				uint32 SetQuestParameter(Logic::Player* player, uint32 questHash, uint8 paramIndex, uint32 newValue);

				//uint16 GetItemCount(Logic::Player* player, uint32 hash)=0;
				uint32 GetItemCount(Logic::Player* player, const Database::Info::ItemInfo::IFindCondition& condition) const;

				void ChangePlayerStatusLimit(Logic::Player* player, const std::vector<uint32>& limits);
				void ChangePlayerStatusRate(Logic::Player* player, const std::vector<float>& rate);

				void LocPlayers(bool lock);
				void SetFloor(int floor);
				void CheckStateBundleForAchievement(uint32 stateBundleHash);
				void DisplayTextEvent(Constants::DisplayTextType type, uint16 textId, float param);
				void NoticeTextEvent(Logic::Player* player, Constants::DisplayTextType type, uint16 textId, float param);
				void DisplayText(Constants::DisplayTextType type, const String& message);
				void NoticeText(Logic::Player* player, Constants::DisplayTextType type, const String& message);
				void DisplayTimer(Constants::DisplayTimerType timer, float seconds);
				void MissionClear(bool success, uint8 team, const CriticalStatistics::StageStatistics& SS, uint32 dropCount);
				void BroadcastEvent(int eventId, const String& param);
				void NoticeEvent(Logic::Player* player, int eventId, const String& param);
				void NoticeClientStageEvent(Logic::Player* player, int eventId, int eventParam);
				void TeamChange(Serial object, int team);
				void AddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type);
				void NoticeAddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type, Logic::Player* player);
				void RemoveMinimapPing(int pingId);

				void PlayerDie(Serial objectSerial, Serial fromSerial);
				void PlayerSave(Serial objectSerial, Serial fromSerial);
				void StyplePointType(Serial objectSerial, XRated::Constants::StylePointType type);
				virtual	void ContinueCountDown();


				void Paused(bool bPaused);

				void GiveLicense(Logic::Player* player, uint32 licenseGroup, uint32 accessLevel);
				void GiveCharacterLicense(Logic::Player* player, uint32 characterId);

				void SectorChanged(Serial serial, Logic::Sector* sector, Constants::Direction direction);

				const XRated::PvpRoomInfo::BattleGroundInfoType& GetBattleGroundInfo() const;
				int GetKillCount(uint32 serial);
				int GetDeathCount(uint32 serial);

				bool Rebirthed(Logic::Player& player);
				void RebirthRollBacked(Logic::Player& player);

				void NoticeStylePointStateToAllUser(XRated::StylePoint::State state, uint8 team = 255);

				void GambleGameEnd(const std::vector<uint8>& rankList);
				void GambleNextGame(Logic::Player* player, float seconds);
				void GambleNextGame(float seconds);

				const StageLicense& GetCurrentStageLicense();

				int GetSlimeRaceIntervalInMin() const;

				//Holiday Event
				bool IsHolidayEventTime(uint32 eventId);

				//Single only
#ifdef _SINGLE
				void NPCDebugMessage(Serial serial, String msg);//gui->Chat(serial, Constants::ChatType::Global, msg);				
#endif

				void PetInfoUpdated(Logic::Player* player, const XRated::Pet& pet, const XRated::Serial petSerial, const String& petExpression);
				void PetCaredBySchool(Logic::Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet);
				void PetTakenOutFromSchool(Logic::Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet);
				void PetRevived(Logic::Player* player);
				void PetChange(Logic::Player* player, uint32 petItemHash, XRated::GlobalSerial petItemSerial);
				void PetDeleted(Logic::Player* player, XRated::GlobalSerial petSerial);
				void PetItemEquipped(bool bequipped, XRated::GlobalSerial petSerial, XRated::Serial petObjectSerial, Logic::Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId);
				void AddedSkillPointPlus(Logic::Player* player, uint16 currentSkillPoint, uint16 skillPointPlus, uint16 storedSkillPoint);

				void AddedGuildUser(Serial player);
				void RemovedGuildUser(Serial player);
				void UpdateGuildExpFactor(int guildId);
				void ChangeWeatherToNone(const float fadeIntime);
				void ChangeWeatherToClear(const float fadeIntime);
				void ChangeWeatherToRain(const float fadeIntime);
				void ChangeWeatherToSnow(const float fadeIntime);
				void ChangeWeatherToAqua(const float fadeIntime);
			private:
				bool SetStage(StageLicense& targetStage, const std::string& roomPass, const int64& pActivationSerial, const String& userName);
				bool SideStageJoinCheck() const;
				bool CapacityCheck(UserSharedPtr user) const;
				uint16 GetStageCapcacity() const;
				void SetStylePointUserCount(const uint16& count);
			public: //IUserRoom
				void DebugCommand(User& user, const String& msg);
				void SpectatorChat(const String& characterName, Protocol::ToServer::Chat& chat);
				void Chat(const uint64& serial, Protocol::ToServer::Chat& chat);
				void Voice(const uint64& serial, Protocol::ToServer::Voice& voice);
				void PartyInvite(UserSharedPtr user, const uint64& serial);
				void PartyAccept(UserSharedPtr accepter, UserSharedPtr inviter);
				void PartyLeave(UserSharedPtr user);
				void NotifySpectatorLeft(UserSharedPtr user);
				bool Command(XRated::Logic::Player* player, XRated::Constants::Command command, XRated::Constants::Direction dir); ///< @reutrn false if the command is invalid
				void Cast(XRated::Logic::Player* player, uint32 skill);
				void LogicSkillPointUp(XRated::Logic::Player* player, Protocol::ToServer::SetSkillLevel& setskilllevel);
				void AddSkillPointPlus(XRated::Logic::Player* player, uint16 skillPointPlus);
				void Revive(XRated::Logic::Player* player, bool self = true);
				void DailyRevive(XRated::Logic::Player* player);
				void InstantCoinRevive(XRated::Logic::Player* player, uint64 orderNumber);

				void CreatePet(XRated::Logic::Player* player, const XRated::Pet& pet);
				void GivePresentToPet(XRated::Logic::Player* player, XRated::GlobalSerial petSerial, uint32 sellPrice, uint32 count);

				void addminimapping(const uint64& serial, float3 posotion);
				bool Equip(XRated::Logic::Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields instance, XRated::Constants::Equipment pos);
				bool EquipToEquip(Logic::Player* player, XRated::Constants::Equipment from, XRated::Constants::Equipment to);
				bool SwapEquip(Logic::Player* player, const std::vector< EquippedItem >& newEquipments);
				bool SwapCashEquip(Logic::Player* player, const std::vector< EquippedItem >& newEquipments);
				void ValidateEquippedItems(Logic::Player* player);
				bool Use(XRated::Logic::Player* player, uint32 id, int bag, int pos);
				void SetLogicUserPvpRank(Logic::Player* player, const uint32 pvpRank);

				bool PetItemUse(XRated::Logic::Player* player, uint32 id, int bag, int pos, XRated::GlobalSerial serial);
				void DropPetItem(XRated::Logic::Player* player, XRated::GlobalSerial serial);
				void PetFeed(XRated::Logic::Player* player, const XRated::GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed = false);
				void PetLevelDecrease(Logic::Player* player, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel);
				bool PetItemsEquip(Logic::Player* player, XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields instance, uint32 equipPos);

				void TakeCarePetBySchool(Logic::Player* player
					, const uint32& petItemHash, const XRated::GlobalSerial& petSerial
					, const Database::Enchant::EnchantBitfields& instance, const uint8& count
					, const uint32& period, const float& expFactor);
				void TakeOutCaredPet(Logic::Player* player, const XRated::GlobalSerial& petSerial);
				void PetRenaming(Serial playerSerial, const XRated::GlobalSerial petSerial, const String& newName);
				void UnsummonPet(Logic::Player* player, const XRated::Serial playerSerial);

				void VoteRequest(UserSharedPtr user, Protocol::ToServer::RequestVoting& requestvoting);
				void Vote(UserSharedPtr user, Constants::VoteResult vote);
				void PersonalVoteRequest(UserSharedPtr user, Protocol::ToServer::RequestPersonalVoting& requestvoting);
				void PersonalVoteClear(UserSharedPtr user, Protocol::ToServer::AnswerPersonalVoting& requestvoting);
				void EnterShop(const uint64& serial, Constants::ShopType shop, uint32 param);
				void LeaveShop(const uint64& serial, float3 position, float3 direction);
				void JoinEndUser(UserSharedPtr user, float progress);
				Common::ROOMKIND GetRoomKind() const;
				const Database::Info::StageGroup* GetCurrentStageGroupInfo() const;
				StageLocation GetCurrentStage() const;
				void GiveUpRevive(Logic::Player* player);

				void DecreaseSkillPoint(Logic::Player* player, uint32 skillGroupHash);

				void ResetSkillPoint(Logic::Player* player, uint32 skillGroupHash);

				UserSharedPtr GetUser(const uint64& serial);
				void SendToAll(Serializer::ISerializable& packet);

				void AddExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp, bool withFactor);
				void AddPvpExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp);
				void AddWarExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp);
				void AddStateBundle(User& user, uint32 hash);
				void AddPassive(User& uer, uint32 hash);
				void RemovePassive(User& user, uint32 hash);

				void FamiliarCommand(User& user, uint16 index, XRated::Serial who, Constants::Familiar::Command command);
				void ChangeFamiliarFormation(User& user, Constants::Familiar::Formation formation);

				bool IsMissionCleared() const;
				bool IsNowCampfire() const;
				float GetObjectDistance(User& user, uint32 hash) const;
				void CreateItem(User& user, const XRated::RewardItem& rewardItem, bool isPrivateItem = false);
				int	 GetProprietyLevel();
				void ChangeStylePointStateToLogic(Logic::Player* player, XRated::StylePoint::State state);
				void CashItemView(UserSharedPtr user, std::pair< uint16, uint16 > flag);
				std::pair< uint16, uint16 > GetCashItemViewFlag(const UserSharedPtr user);
				void SendReinforcementResult(UserSharedPtr user, ItemPosition itemPos, uint32 hash, int64 oldInstance, int64 newInstance);
				void SendLightReinforcementResult(UserSharedPtr user, ItemPosition itemPos, uint32 hash, int64 oldInstance, int64 newInstance);

				uint32	GetRoomIndex() const;
				uint16	GetRoomID() const;

				//Fishing
				void RequestFishingList(UserSharedPtr user);
				Database::Info::StageInfo* GetStageInfo() const;
				void FishingInfo(UserSharedPtr user, uint32 hash, uint8 baitCnt, int32 rare);
				void AddFishingUser(uint32 serial);
				void RemoveFishingUser(uint32 serial);
				int GetFishingUserCnt() const;
				void SendAllFishingUser(UserSharedPtr user);

				void StageGiveUpLog(UserSharedPtr user);

				//Gamble - SlimeRace
				void SlimeRaceBet(UserSharedPtr user, Protocol::ToServer::Gamble::SlimeRace::Bet& packet);
				void SlimeRaceClearBet(UserSharedPtr user, Protocol::ToServer::Gamble::SlimeRace::ClearBet& packet);
				void SlimeRaceRequestBettingState(UserSharedPtr user);
				void SlimeRaceRequestRecentResults(UserSharedPtr user);

				void SetPlayTimePenalty(Logic::Player* player, XRated::Constants::PlayTimePenalty::Type flag);

				void ChangedExpFactorFromItem(User& user, XRated::Constants::ExpFactorCategoryFromItem category, float factor);
				void ChangedExpFactor(User& user);
				void AddGuildUser(User& user);
				void RemoveGuildUser(User& user);

				/// Rebirth
				bool Rebirth(const User& user, uint32 levelAfterRebirth);
				bool RebirthRollBack(const User& user);
				void NotifyRebirth(XRated::Serial serial, uint16 level, uint16 rebirthCount, uint16 storedLevel, uint16 storedSkillPoint, const std::vector<XRated::StageLicense>& updatedLicense, const DateTime& lastRebirthDateTime);

				Protocol::FromServer::Family::InviteResult::Type RequestInviteFamily(XRated::Family::FamilySerial familySerial, DateTime createdDate, XRated::Serial targetSerial, const String& targetName, const String& invter);
				Protocol::FromServer::Family::InviteResult::Type RequestInviteFamily(XRated::Serial targetSerial, const String& targetName, const String& invter);


				bool CompletedQuest(User& user, uint32 questHash);
				bool AcceptedQuest(User& user, uint32 questHash);

				uint32 ExcuteRoomSerialUsers(IExcuter& excuter);
			private:
				bool m_NowCampfire = false;
				bool m_Active = false;
				bool m_Loading = false;
				bool m_Locked = false;
				bool m_WaitRoomDelete = false;
				bool m_OpenRoom = false;
				bool m_MissionCleared = false;
				bool m_EnabledPartyJoin = false;

				uint16 m_RoomIndex = 0;
				uint16 m_ThreadIndex = -1;
				uint16 m_LastThreadIndex = 0;
				uint16 m_CurrentId = 0;
				uint16 m_CurrentFloor = 0;
				uint16 m_StartingUserCount = 0;
				uint16 m_CountReviveLimit = 0;
				uint16 m_PartyDeathCount = 0;
				uint32 m_SkillUseCount = 0;

				uint32 m_FloorClearedTime = 0;

				int64 m_ActivationSerial = -1;

				float m_WaitRoomDeleteTime = -1.0f;
				float m_LoadingTime = 0.0f;

				String m_PartyChannelName = L"";

				StageLicense m_CurrentStage;
				Database::Info::StageGroup* m_StageGroupInfo;
				Database::Info::StageInfo* m_StageInfo;

				Common::ROOMKIND m_RoomKind;
				Logic::ILogic* m_Logic;
				RoomUserManager m_UserManager;
				std::list<UserSharedPtr> m_WaitingUsers;

				std::map<uint64, uint16> m_DieCountSerial;
				std::map<uint64, uint16> m_KillCountSerial;

				std::mutex m_Mtx;

				struct Pvp
				{
					bool GameStarted;
					bool BgmStarted;
					int	SyncStartUserCnt;
					int	SyncEnteredUserCnt;
					bool bSendStartEvent;
				} pvp;
				//BattleGround
				PvpRoomInfo::BattleGroundInfoType m_BattleGroundInfo;
				typedef std::vector<Guild> Guilds;
				Guilds m_Guilds;
			};
			typedef std::shared_ptr<Room> RoomSharedPtr;
		}
	}
}