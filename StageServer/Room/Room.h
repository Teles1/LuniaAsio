#pragma once
#include <Logic/Logic.h>
#include <Logic/Object/Player.h>
#include <StageServer/Room/IUpdateRoom.h>
#include <StageServer/Common.h>
#include <StageServer/User/RoomUserManager.h>
#include <StageServer/Room/Services/Guild.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer{
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class Room
				: public IUpdateRoom
				, Logic::ILogic::IEventListener
				, std::enable_shared_from_this<Room>
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
				Common::ROOMKIND GetRoomKind() const;
				Constants::GameTypes GetRoomType() const;
			public:
				uint16 UserCount() const;
				void Clear();
			public: // IEventListener implementation which sends information from the Logic.
				void Initialized(Database::Info::StageInfo* info, uint16 uniqueId);
				std::mutex& GetSyncRoom();
				void NonPlayerCreated(NonPlayerData& data);
				bool Tamed(Logic::Player* player, Serial familiar, Constants::Familiar::Type type);
				void StructureCreated(CharacterData& data);
				void VehicleCreated(CharacterData& data);
				void ProjectileCreated(ObjectData& obj, const float3& targetPos, Serial target, const std::wstring& owner);
				void ItemCreated(ObjectData& data, const std::wstring& owner, float dt, uint16 stackCount, bool isPrivateItem);
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
				void DisplayText(Constants::DisplayTextType type, const std::wstring& message);
				void NoticeText(Logic::Player* player, Constants::DisplayTextType type, const std::wstring& message);
				void DisplayTimer(Constants::DisplayTimerType timer, float seconds);
				void MissionClear(bool success, uint8 team, const CriticalStatistics::StageStatistics& SS, uint32 dropCount);
				void BroadcastEvent(int eventId, const std::wstring& param);
				void NoticeEvent(Logic::Player* player, int eventId, const std::wstring& param);
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
				void NPCDebugMessage(Serial serial, std::wstring msg);//gui->Chat(serial, Constants::ChatType::Global, msg);				
#endif

				void PetInfoUpdated(Logic::Player* player, const XRated::Pet& pet, const XRated::Serial petSerial, const std::wstring& petExpression);
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
			private:
				bool SetStage(StageLicense& targetStage, const std::string& roomPass, const int64& pActivationSerial, const std::wstring& userName);
				bool SideStageJoinCheck() const;
				bool CapacityCheck(UserSharedPtr user) const;
				uint16 GetStageCapcacity() const;
			};
			typedef std::shared_ptr<Room> RoomSharedPtr;
		}
	}
}