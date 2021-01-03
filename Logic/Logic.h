#pragma once
#include <Info/Info/PvPRewardItemInfos/PvPRewardItemInfosManager.h>
#include <Info/Info/Enchant/Enchant.h>
#include "GameState.h"

namespace Lunia { 
	namespace XRated {
	namespace Logic {

		class Player;

		/**
		@brief	updates game stage database.
		*/
		class ILogic
		{
		public:
			struct Config
			{
				int NumberOfBossRewardUnidentifiedItem;

				inline Config()
					: NumberOfBossRewardUnidentifiedItem(1)
				{
				}
			};

			struct PlayerInitialData/* : public Serializer::ISerializable*/
			{
				std::mutex SyncRoot;
				std::wstring name;
				Lunia::XRated::Constants::ClassType type;
				uint32 xp;
				uint16 level;
				uint32 pvpXp;
				uint16 pvpLevel;
				uint32 warXp;
				uint32 warLevel;
                uint16 storedLevel;
				uint16 rebirthCount;
				uint16 storedSkillPoint;
				uint32 money;
				uint32 bankMoney;
				uint16 life;
				uint16 bonusLife;
				uint16 skillpoint;
				uint32 ladderPoint;
				uint16 ladderMatchCount;
				uint32 ladderWinCount;
				uint32 ladderLoseCount;
				uint32 achievementScore;
				uint16 addedSkillPointPlus;
				uint8 team;
				XRated::CharacterStateFlags characterStateFlags;
				std::shared_ptr<void> user;

				struct FactorGroup
				{
					inline FactorGroup() : ExpFactor(1.0f), MaxHp(1.0f), MaxMp(1.0f) {}

					float ExpFactor;
					float MaxHp;
					float MaxMp;
				} Factors;

				std::vector<Lunia::XRated::Skill> skills;
				std::vector< std::pair<const Database::Info::ItemInfo*, int64/*enchant bitfields or normal bitfields*/> > equipments;
				//std::vector< Lunia::XRated::Quest > quests;

				struct StartPositionTag
				{
                    bool UseCustomStartPoint;
					Database::Info::StageInfo::Placement Place;
                    
					inline StartPositionTag() : UseCustomStartPoint(false) { }
					inline StartPositionTag(const Database::Info::StageInfo::Placement& place) : UseCustomStartPoint(true), Place(place) { }
				} StartPosition;

				std::vector<Pet>		pets;
				std::vector<PetCaredBySchool> petCared;

				Lunia::DateTime lastRebirthDateTime;

				PlayerInitialData& operator=(const PlayerInitialData& rhs);
			};

			struct UpdateInfo {
				bool bUpdated;
				double updateTime;
				double processCommandTime;
				double largest, largestCmd;
				int playerCnt, npcCnt, projectileCnt, miscCnt;
				int playerCntBefore, npcCntBefore, projectileCntBefore, miscCntBefore;
				int cmdCount;

				inline void Init() {
					bUpdated = false; updateTime=processCommandTime=largest=largestCmd=0;
					playerCnt=npcCnt=projectileCnt=miscCnt=cmdCount=0;
					playerCntBefore=npcCntBefore=projectileCntBefore=miscCntBefore=0;
				}
				inline void SetProcessCommandTime(double time) { if ( largestCmd<time) largestCmd=time; processCommandTime=time; }
				inline void SetProcessTime(double time) { if (largest<time) largest=time; updateTime=time; }
			};

			struct TamingInfo : public Serializer::ISerializable {
				Serial owner;
				Serial tamed;
				Constants::Familiar::Type type;

			public :
				inline TamingInfo(Serial o, Serial t, Constants::Familiar::Type tp) : owner(o), tamed(t), type(tp) {}

			public :
				void Serialize(Lunia::Serializer::IStreamWriter& out) const;
				void Deserialize(Lunia::Serializer::IStreamReader& in);
			};

			enum InstantCoinReviveError
			{
				NoError = 0,
				RollbackRequested,
				PvPBlocked,
			};
				
			struct IEventListener /* Server will broadcast these events */
			{
				virtual ~IEventListener() {}

				virtual void Initialized(Database::Info::StageInfo* info, uint16 uniqueId)=0;
				virtual std::mutex &GetSyncRoom() = 0;
				virtual void NonPlayerCreated(Lunia::XRated::NonPlayerData& data)=0;
				virtual bool Tamed(Player* player, Serial familiar, Lunia::XRated::Constants::Familiar::Type type)=0;
				virtual void StructureCreated(Lunia::XRated::CharacterData& data)=0;
				virtual void VehicleCreated(Lunia::XRated::CharacterData& data)=0;
				virtual void ProjectileCreated(Lunia::XRated::ObjectData& obj, const float3& targetPos, Serial target, const std::wstring& owner)=0;
				virtual void ItemCreated(Lunia::XRated::ObjectData& data, const std::wstring& owner, float dt, uint16 stackCount, bool isPrivateItem)=0;
				virtual void ObjectCreated(Lunia::XRated::ObjectData& data)=0;
				virtual void ObjectDestroyed(Serial gameObjectSerial, Lunia::XRated::Constants::ObjectType type, uint32 hash, bool silent, uint8 team=255, Lunia::XRated::NonPlayerData::NpcType npcType=Lunia::XRated::NonPlayerData::NpcType::Normal)=0;

				virtual void Died(Player* player)=0;
				virtual void Revived(Player* player, uint16 totalReviveCount)=0;
				virtual void InstantCoinRevived( Player* player, InstantCoinReviveError error, uint64 orderNumber )=0;
				
				virtual bool IsPvPRoom() = 0;

				virtual void AnimationChanged(Serial gameObjectSerial, uint32 animation, const float3& position, const float3& direction, const float param)=0;
				virtual void CollisionStateChanged(Serial gameObjectSerial, bool collision, const float3& position)=0;

				virtual void XpGained(Player* player, XRated::Constants::ExpAcquiredType type, uint64 storyXp, int32 pvpXp=0, int32 warXp=0, Serial beKilledNpc=0)=0;
				virtual void GoldGained(Player* player, XRated::Constants::GoldRewardAcquiredType type, uint32 money)=0;
				virtual bool AcquireItem(Player* player, uint32 itemId, Serial itemSerial, uint16 stackCount, InstanceEx instance)=0;

				virtual void SendPvPRewardItemInfo( Database::Info::PvPRewardInfo rewardInfo )=0;
				virtual void AcquireItemWithMailSupport( Player* player, uint32 itemId, uint16 stackCount )=0;

				/* Async Events for player commands*/
				virtual bool PlayerCreated(Player* player)=0;
				virtual void JoinFailed(std::shared_ptr<void> user)=0;
				virtual void ItemUsed(uint32 returnValue/*Logic::Player::ItemUseResult*/, Player* player, const Database::Info::ItemInfo* info, int bag, int pos)=0;
				virtual void Casted(Serial gameObjectSerial, uint32 skillGroupName, uint8 skilllevel)=0;
				virtual void ItemEquipped(bool bequipped, Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId)=0;
				virtual void EquipementSwapped(bool bequipped, Player* player, const std::vector< EquippedItem>& newEquipments ) = 0;
                virtual void CashEquipementSwapped(bool bequipped, Player* player, const std::vector< EquippedItem>& newEquipments ) = 0;				
				virtual void ItemEquipToEquipChanged(bool bChanged,Player* player,Lunia::XRated::Constants::Equipment from, Lunia::XRated::Constants::Equipment to) =0;
				//virtual void SkillPointIncreased(bool bIncreased, Player* player, uint32 skillGroup, int level)=0; ///< @remarks skillGroup parameter can be 0 as all skills
				virtual void SkillPointChanged(Player* player, uint32 skillGroupHash, int level)=0; ///< skillGroupHash parameter can be 0 as all skills. and level can be less than 0 by setting failure


				// object state changed
				virtual void LevelUp(Player* player, XRated::Constants::ExpType expType, uint16 level, uint32 storedLevel)=0;
				virtual void SkillPointGot(Player* player, int skillPoint)=0;
				virtual void StateChanged(Serial gameObjectSerial, uint32 id, int level, Serial byWhom=0)=0;
				virtual void StatusChanged(Serial gameObjectSerial, float3 pos, float3 dir, float hp, float mp, Serial byWhom, uint32 byWhat, uint32 sFlag, uint32 airComboCount = 0)=0;

				virtual bool AddItem(Player* player, uint32 id, Serial gameObjectSerial, int cnt)=0;
				virtual bool AddItem(Player* player, Serial gameObjectSerial, int* arrayId, int* arrayCnt)=0;
				virtual bool RemoveItem(Player* player, uint32 id, int cnt)=0;
				virtual bool RemoveItem(Player* player, int* arrayId, int* arrayCnt)=0;
				virtual bool ExamineItemSlotFromPlayer(Player* player, Database::Info::ItemInfo* info, int cnt)=0;

				virtual void AcquirePvpItem(Player* player, uint32 itemHash)=0;
				virtual void UsePvpItem(Player* player, uint32 itemHash)=0;
				virtual void DropPvpItem(Player* player)=0;

				//Quest
				virtual uint8 GetQuestState(Player* player, uint32 questHash)=0;
				virtual uint8 SetQuestState(Player* player, uint32 questHash, uint8 newState)=0;
				virtual uint32 GetQuestParameter(Player* player, uint32 questHash, uint8 paramIndex)=0;
				virtual uint32 SetQuestParameter(Player* player, uint32 questHash, uint8 paramIndex, uint32 newValue)=0;

				//virtual uint16 GetItemCount(Player* player, uint32 hash)=0;
				virtual uint32 GetItemCount(Player* player, const Database::Info::ItemInfo::IFindCondition& condition) const=0;

				virtual void ChangePlayerStatusLimit( Player* player, const std::vector<uint32>& limits )=0;
				virtual void ChangePlayerStatusRate( Player* player, const std::vector<float>& rate )=0;

				virtual void LocPlayers(bool lock)=0;
				virtual void SetFloor( int floor )=0;
				virtual void CheckStateBundleForAchievement( uint32 stateBundleHash )=0;
				virtual void DisplayTextEvent(Constants::DisplayTextType type, uint16 textId, float param)=0;
				virtual void NoticeTextEvent(Player* player, Constants::DisplayTextType type, uint16 textId, float param)=0;
				virtual void DisplayText(Constants::DisplayTextType type, const std::wstring& message)=0;
				virtual void NoticeText(Player* player, Constants::DisplayTextType type, const std::wstring& message)=0;
				virtual void DisplayTimer(Constants::DisplayTimerType timer, float seconds)=0;
				virtual void MissionClear(bool success, uint8 team, const CriticalStatistics::StageStatistics& SS, uint32 dropCount)=0;
				virtual void BroadcastEvent(int eventId, const std::wstring& param)=0;
				virtual void NoticeEvent(Player* player, int eventId, const std::wstring& param)=0;
				virtual void NoticeClientStageEvent( Player* player, int eventId, int eventParam ) = 0;
				virtual void TeamChange(Serial object, int team)=0;
				virtual void AddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type)=0;
				virtual void NoticeAddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type, Player* player)=0;
				virtual void RemoveMinimapPing(int pingId)=0;

				virtual void PlayerDie(Serial objectSerial, Serial fromSerial) = 0;
				virtual void PlayerSave(Serial objectSerial, Serial fromSerial) = 0;
				virtual void StyplePointType(Serial objectSerial, XRated::Constants::StylePointType type) = 0;
				virtual	void ContinueCountDown() = 0;


				virtual void Paused(bool bPaused)=0;

				virtual void GiveLicense(Player* player, uint32 licenseGroup, uint32 accessLevel)=0;
				virtual void GiveCharacterLicense(Player* player, uint32 characterId)=0;

				virtual void SectorChanged(Serial serial, Sector* sector, Constants::Direction direction)=0;

				virtual const XRated::PvpRoomInfo::BattleGroundInfoType& GetBattleGroundInfo() const =0;
				virtual int GetKillCount(uint32 serial)=0;
				virtual int GetDeathCount(uint32 serial)=0;

				virtual bool Rebirthed(Player& player) = 0;
				virtual void RebirthRollBacked(Player& player) = 0;
		
				virtual void NoticeStylePointStateToAllUser(XRated::StylePoint::State state, uint8 team=255)=0;

				virtual void GambleGameEnd(const std::vector<uint8>& rankList)=0;
				virtual void GambleNextGame(Player* player, float seconds)=0;
				virtual void GambleNextGame(float seconds)=0;

				virtual const Lunia::XRated::StageLicense& GetCurrentStageLicense()=0;

				virtual int GetSlimeRaceIntervalInMin() const = 0;

				/* Holiday Event */
				virtual bool IsHolidayEventTime(uint32 eventId)= 0;

				//Single only
#ifdef _SINGLE
				virtual void NPCDebugMessage(Serial serial, std::wstring msg)=0;//gui->Chat(serial, Constants::ChatType::Global, msg);				
#endif

				virtual void PetInfoUpdated(Player* player,const XRated::Pet& pet, const XRated::Serial petSerial, const std::wstring& petExpression) = 0;
				virtual void PetCaredBySchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet ) = 0;
				virtual void PetTakenOutFromSchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet ) = 0;
				virtual void PetRevived( Player* player ) = 0;
				virtual void PetChange(Player* player, uint32 petItemHash, XRated::GlobalSerial petItemSerial) = 0;
				virtual void PetDeleted(Player* player, XRated::GlobalSerial petSerial) =0;
				virtual void PetItemEquipped(bool bequipped/*����or ����*/, XRated::GlobalSerial petSerial, XRated::Serial petObjectSerial, Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId)=0;
				virtual void AddedSkillPointPlus(Player* player,uint16 currentSkillPoint, uint16 skillPointPlus, uint16 storedSkillPoint) = 0;

				virtual void AddedGuildUser(Serial player) = 0;
				virtual void RemovedGuildUser(Serial player) = 0;
				virtual void UpdateGuildExpFactor(int guildId) = 0;
				virtual void ChangeWeatherToNone( const float fadeIntime ) = 0;
				virtual void ChangeWeatherToClear( const float fadeIntime ) = 0;
				virtual void ChangeWeatherToRain( const float fadeIntime ) = 0;
				virtual void ChangeWeatherToSnow( const float fadeIntime ) = 0;
				virtual void ChangeWeatherToAqua( const float fadeIntime ) = 0;
			};

			virtual ~ILogic() {}

			virtual void SetEventListener(IEventListener& listener)=0;
			virtual IEventListener& GetEventListener()=0;
			//virtual void SetCriticalSection(Lunia::CriticalSection& cs)=0;
			/** initlize stage */
			virtual void Init(uint32 stageGroupHash, uint16 accessLevel, uint16 uniqueId )=0;
			//virtual void RehashAll()=0;
			virtual void Pause(float time)=0;
			virtual void Resume()=0;
			virtual void GameStart()=0;

			/* asynchronous commands **************************************************************/
			// player commands
			virtual void Join(PlayerInitialData* player, std::shared_ptr<void> user)=0;
			virtual void ChangeTeam(Player* player, int teamSerial)=0;
			virtual void Part(Player* player)=0;
			virtual void Command(Player* player, Constants::Command command, const Constants::Direction dir)=0; // Command sends action only, move, stop, dash, attackA and attackB
			virtual void Cast(Player* player, uint32 id)=0;
			virtual bool Use(Player* player, Database::Info::ItemInfo* info, int bag, int pos)=0;
			virtual	void SetLogicUserPvpRank(Player* player, const uint32 pvpRank)=0;

			virtual bool PetItemUse(Player* player, Database::Info::ItemInfo* info, int bag, int pos, GlobalSerial serial)=0;
			virtual void DropPetItem(Player* player, GlobalSerial itemSerial) =0;			
			virtual void PetFeed( Player* player, const GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed = false ) =0;
			virtual void GivePresentToPet( Player* player, GlobalSerial petSerial, uint32 sellPrice, uint32 count ) =0;
			virtual void PetLevelDecrease(Player* player, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel) = 0;
			virtual void CreatePet( Player* player, const Pet& pet ) = 0;

			virtual bool PetEquip(XRated::GlobalSerial petSerial, Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where)=0;
			virtual void TakeCarePetBySchool( Player* player
											, const uint32& petItemHash, const XRated::GlobalSerial& petSerial
											, const Database::Enchant::EnchantBitfields& instance, const uint8& count
											, const uint32& period, const float& expFactor )=0;
			virtual void TakeOutCaredPet( Player* player, const XRated::GlobalSerial& petSerial ) = 0;
			virtual void PetRenaming(Serial playerSerial, const XRated::GlobalSerial petSerial, const std::wstring& newName) = 0;
			virtual void UnsummonPet(Serial playerSerial) = 0;

			virtual bool Equip(Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields fields, Constants::Equipment where)=0;
			virtual bool ChangeEquipToEquip(Player* player, XRated::Constants::Equipment from, XRated::Constants::Equipment to) =0;
			virtual bool SwapEquip( Player* player, const std::vector< EquippedItem > & newEquipments ) = 0;
			virtual bool SwapCashEquip( Player* player, const std::vector< EquippedItem > & newEquipments ) = 0;
			virtual void Revive(Player* player,bool self = true)=0;
			virtual void DailyRevive(Player* player) = 0;
			virtual void InstantCoinRevive( Player* player, uint64 orderNumber ) = 0;
			virtual void IncreaseSkillPoint(Player* player, uint32 skill)=0;
			virtual void DecreaseSkillPoint(Player* player, uint32 skill)=0;
			virtual void AddSkillPointPlus(Player* player, uint16 skillPointPlus) = 0;
			virtual void ResetSkillPoint(Player* player, uint32 skillGroupHash)=0; ///< if skillGroupHash is 0, means all of the skills of the player.
			virtual bool IsAbleToBuy(Player* player, Constants::ShopType shopType) = 0;
			virtual bool IsAbleToTrade(Player* player, Serial target) = 0;
			virtual void SetPlayerExpFactor(Player* player, float factor) =0;
			virtual void GetPlayerSkills(uint32 playerSerial, std::vector<Skill>& skills)=0;		
			virtual void SetPlayTimePenalty(Player* player, XRated::Constants::PlayTimePenalty::Type flag) =0;
			virtual void ValidateEquippedItems(Player* player) =0;

			// Rebirth
			virtual void Rebirth(XRated::Serial playerSerial,uint32 levelAfterRebirth) = 0;
			virtual void RebirthRollBack(XRated::Serial playerSerial) = 0;

			// internal commands
			virtual void CreateItem(float3 position, std::wstring itemId, std::wstring ownerId)=0; ///< not used
			virtual void CreateItem(Player* player, const XRated::RewardItem& rewardItem, bool isPrivateItem=false)=0;

			// Quest commands
			virtual void AddExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)=0;
			virtual void AddExpWithNoFactor(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)=0;
			virtual void AddPvpExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)=0;
			virtual void AddWarExp(XRated::Constants::ExpAcquiredType type, Player* player, uint32 exp)=0;

			virtual void AddStateBundle(Player* player, uint32 hash)=0;
			virtual void AddPassive(Player* player, uint32 hash) =0;
			virtual void RemovePassive(Player* player, uint32 hash) =0;


			virtual void AddGuildUser(Player& player) =0;
			virtual void RemoveGuildUser(Player& player) =0;
			virtual void RequestUpdateGuildExpFactor(Player& player, int guildId) =0;

			//Familiar commands
			virtual void FamiliarCommand(Player* player, uint16 index, XRated::Serial who, Constants::Familiar::Command command)=0;
			virtual void ChangeFamiliarFormation(Player* player, Constants::Familiar::Formation formation)=0;

			virtual void GiveUpRevive(Player* player)=0;

			//StylePoint
			virtual void ChangeStylePointState(Player* player, XRated::StylePoint::State state)=0;
			virtual void ChangeStylePointPlayerCount(Player* player, int count)=0;
			/************************************************************** asynchronous commands */


			//Find first npc which has same hash with second parameter 'hash'.
			//Return distance between player and found npc. If cannot find matching npc, return -1.
			virtual float GetObjectDistance(Player* player, uint32 hash) const=0;

			/* helpers */
			virtual double GetTickResolution() const=0;
			virtual float GetIdleTime() const=0;
			virtual float ElapsedTime()=0;
			virtual std::vector<Lunia::XRated::ObjectData*> GetObjectList()=0;
			virtual std::vector<Lunia::XRated::ItemData*> GetItemList()=0;
			virtual std::vector<Lunia::XRated::NonPlayerData*> GetNonPlayerList()=0;
			virtual std::vector<Lunia::XRated::PlayerData*> GetPlayerList()=0;
			virtual void GetTamingList(std::vector<Lunia::XRated::Logic::ILogic::TamingInfo>& l)=0;
			virtual bool IsLoading()=0;
			virtual float2 GetStartingPoint()=0;
			virtual bool IsActivated() const=0;

			virtual bool IsStageEnded()=0;
			virtual float GetElpasedTime()=0;

			/** Update a turn */
			virtual void Update(double dt)=0;
			virtual const UpdateInfo& GetUpdateInfo() const=0;
            
			virtual const ObjectData& GetObjectData(Serial gameObjectSerial)=0;


			virtual std::wstring DebugCommand(uint32 serial, const std::wstring& command, const std::wstring& param)=0;

			virtual void GetCurrentBattleGroundInfo( std::map<uint8 , uint8 >& killcount , float& remainTime )=0;
			virtual void CompletedQuest(Serial player, uint32 questHash ) =0;
			virtual void AcceptedQuest(Serial player, uint32 questHash ) = 0;
			
			virtual void NoticeHolidayEvent(uint32 eventId, bool start) = 0; 

#ifdef _SINGLE
			virtual void ReloadScript()=0;
			virtual bool CheckScript()=0;
#endif // ! _SINGLE
		};

		ILogic* CreateLogic();
	}

} }
