#pragma once
#include "Logic.h"
#include "Object/ProjectileManager.h"
#include "Object/ItemManager.h"
#include "Object/Player.h"
#include "AI/TriggerSystem.h"
#include "Stage/Stage.h"
#include <Core/Utils/Math/PseudoRandom.h>

namespace Lunia { namespace XRated {	namespace Logic {

	class GameStateDatabase : public IGameStateDatabase {
	public :
		enum { PCSerialBase=1000, NPCSerialBase=10000, ProjectileSerialBase = 100000};

		const int SmallSectorSize_X, SmallSectorSize_Z;
		const int MaxMapSize_X, MaxMapSize_Z;
	private:

		bool bSquare;

		Stage stage;	//Trigger manager.

		IdleSerialQueue playerSerials;
		Serial npcSeed;
		Serial projectileSeed;

		SectorManager smallSector, bigSector;

		ObjectMap objects;
		PlayerMap players;
		NPCMap npcs;

		ItemManager				itemManager;
		ProjectileManager		projectileManager;

		AI::TriggerSystem		triggerSystem;

		Lunia::Math::PseudoRandomFloat randomFloat;
		Lunia::Math::PseudoRandomInt randomInt;

		bool bUpdateMISC;
		bool bUpdateNPC;

		std::vector<NonPlayer*> npcBuffer;
		std::vector<Actor*> objectBuffer;

		ILogic& logic;

		struct Pvp
		{
			struct DeathMatch
			{
				int sumLv[2];
				float avgLv[2];
				float deter[2];
				int memberCnt;	
			} deathMatch;

			struct BattleGround
			{
				uint8 killCount[2];
				float limitPlayTime; 
			} battleGround ;
		} pvp;
	public:
		void GetCurrentBattleGroundInfo( std::map<uint8 , uint8 >& killcount , float& remainTime ) { 
			killcount[1]=pvp.battleGround.killCount[0];
			killcount[2]=pvp.battleGround.killCount[1];
			
			CriticalStatistics::StageStatistics ss;
			stage.GetStageStatistics( ss );
			remainTime = pvp.battleGround.limitPlayTime < 0 ? -1 : pvp.battleGround.limitPlayTime - ss.ElapsedTime;
		}
	private:

		float airDamage;
		float airDamageByNpc;
		uint32 stageCnt;
		uint32 tickCnt; 

		std::mutex csNPCBuffer, csMISCBuffer;

		mutable std::mutex csUpdateObject;

		uint16	totalReviveCount;

	private: /* helper function */
		bool IsOverlapped(Actor* actor);

	protected:
		inline bool CheckAlliance(Actor* obj, Constants::AllianceType by, uint8 value);

		inline Sector* ChooseSector(float range, Object::Sectors& sectors);

		void AddUnidentifiedItems(Database::Info::NonPlayerInfo* info, uint32 count );

	public:
		GameStateDatabase(ILogic& logic);
		~GameStateDatabase();
		void LoadStage(Database::Info::StageGroup* sGroup, uint32 name, uint16 uniqueId);
		void GameStart();
		bool IsLoading() const { return stage.IsLoading(); }
		bool IsStageEnded() const { return stage.IsMissionCleared(); }
		float GetElpasedTime() { return stage.GetElpasedTime(); }

		void operator=(GameStateDatabase&) {};

	public:
		/* IGameStateDatabase implementaion */

		// ByCaspian
		uint16 GetPlayerCnt(uint8 team) const;
		int GetPlayerSerial( uint8 team, uint16 index ) const;

		uint16 GetPlayablePlayerCnt() const;
		uint16 GetPlayablePlayerCnt(uint8 team) const;
		bool IsPlayable( uint32 playerSerial ) const;
	private:
		static bool IsPlayable(Player& p);
	public:
		
		
		void PlayerLeft( Serial playerSerial, uint8 team ) { stage.PlayerLeft( playerSerial, team ); }
		void PlayerEntered( Serial playerSerial, uint8 team ) { stage.PlayerEntered( playerSerial, team); }



		inline Map::StageMap* GetStageMap() { return stage.GetStageMap(); }
		inline Constants::GameTypes GetGameType() { return stage.GetGameType(); }
		inline int GetPlayerCnt() const { return (int)players.size(); }
		int GetMaxTeamPlayerCnt() const;
		bool IsPlayerTeam( uint8 team ) const;
		int GetActivePlayerCnt() const;
		inline int GetNPCCnt() const { return (int)npcs.size(); }
		inline int GetMISCCnt() const { return (int)objects.size(); }
		inline int GetProjectileCnt() const { return projectileManager.GetCnt(); }

		Object* Remove(Serial serial);
		Object* Retrieve(Serial serial);
		Object* RetrieveByID(uint32 name);
		Actor* RetrieveActor(Serial serial);
		Actor* RetrieveActorByID(uint32 name);
		NonPlayer* RemoveNpc(Serial);
		NonPlayer* RetrieveNPC(Serial);
		NonPlayer* RetrieveNPCByID(uint32 name);
		Player* RemovePlayer(Serial);
		Player* RetrievePlayer(Serial serial);
		Player* RetrievePlayerByID(const std::wstring& id);

		Player* NextPlayer(Serial);
		Player* GetRandomPlayer();

		bool AddToObjectBuffer(uint32 stageCnt, Actor* object);
		Serial CreateObject(Actor* object);
		Serial CreatePlayer(Player*);
		Serial CreatePlayer(Player*, Serial serial);
		bool AddToNPCBuffer(uint32 stageCnt, NonPlayer* npc);
		Serial CreateNonPlayer(NonPlayer* npc);
		void CreateProjectile(uint32 name, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, uint16 toMyTarget, float createTime,Lunia::XRated::Serial creator);
		Serial CreateItem(uint32 name, const float3& pos, const float3& direction, uint16 stackCount=1, InstanceEx instance=0, bool isPrivateItem=false);
		Serial CreateItem(uint32 name, const float3& pos, const float3& direction, uint32 userSerial, uint16 stackCount=1, InstanceEx instance=0, bool isPrivateItem=false);
		void ClearItem();

		AI::TriggerID AddTrigger(uint16 priority, AI::Trigger::TriggerType triggerType, Serial creator,
									Constants::ObjectType objType, float3 pos, float rad, float timeLimit,
									bool bUpdate, bool bImmediately);

		void Clear();

		//Retrieve object list
		void GetAreaObjectRect(const float4& rect, std::vector<Object*>& l);
		void GetAreaObjectRect(const float4& rect, std::vector<Actor*>& l);
		void GetAreaObjectRect(const float4& rect, std::vector<NonPlayer*>& l);
		void GetAreaObjectRect(const float4& rect, std::vector<Player*>& l);

		template<class T> void GetAreaObjectRect(const float4& rect, uint32 id, std::vector<T*>& l);
		template<class T> void GetAreaObjectRect(const float4& rect, const uint8 team, std::vector<T*>& l);
		void GetAreaPlayerByTeam(uint8 team, float3 targetPos, float range, std::vector<Player*>& l);

		void GetAreaObjectForSector9(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l);

		void GetAreaObject(const float3& targetPos, float range, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Projectile*>& l);
		void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l);
		void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l);
		void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l);
		void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l);
		


		const PlayerMap& GetPlayers() { return players; }
		const NPCMap&    GetNPCs() { return npcs; }
		SectorManager& GetSmallSector() { return smallSector; }
		SectorManager& GetBigSector() { return bigSector; }

		float3 GetAvailablePos(const float3& pos, const float& rad);
		float3 GetAvailablePos(const float3& pos, const float& rad, Actor* it);
		float3 GetAvailableGhostPos(const float3& pos, const float& rad);
		float GetRandomFloat() { AutoLock lock(csUpdateObject); return static_cast<float>(randomFloat.Next()); }
		int GetRandomInt(int min, int max) { return (int)randomInt.Next(min, max); }
		Lunia::Math::PseudoRandomFloat* GetRandomClass() { return &randomFloat; }
		bool IsAbletoLocate(Serial serial, const float3& pos, float rad);
		bool IsAbletoLocate(Serial serial, const float3& pos, float rad, Actor* it);
		bool IsAbletoLocateInMap(const float3& pos, float rad);
		bool IsAbletoLocateByGhost(const float3& pos, float rad);

		void RemoveTombstone(Serial ownerSerial);

		template<class T> void GetObjectbyName(uint32 id, std::vector<T*>& l);

		void ObjectMoved(Object*);

		//npc�� path�� Ư�� ����Ʈ�� �����ߴ�.
		void NPCArrived(NonPlayer* npc, int pathCnt, int posCnt);

		//Stage Trigger event
		void Pause(float time);
		void CampfireBegin();
		void FastMatch();

		void GiveUpRevive(Player* player) { stage.CoinPlayerGiveUp(player->GetSerial()); }
		float GetObjectDistance(Player* player, uint32 hash) const; ///<@return minimal distance from the object(hash) and -1 if it does not exist.

		//MissionClear Exp 
		int		GetSecretCount();
		int		GetReviveLimitCount();
		int		GetStandardTime();
		float   GetStandardDiviation();
		int		GetStageProprietyLevel();
		int		GetPlayTime(int level);
		float	GetXpPerSecond(int level, int playTime);
		float	GetMissionClearXp(float xpForSecond);

		//Request from logic	////////////////////////////////////////////////////////////////////////////////////////////////
		int GetNPCCnt();
		void GetObjectList(std::vector<ObjectData*>& l);
		void GetItemList(std::vector<ItemData*>& l);
		void GetNPCList(std::vector<NonPlayerData*>& l);
		void GetPlayerList(std::vector<PlayerData*>& l);
		void GetTamingList(std::vector<Lunia::XRated::Logic::ILogic::TamingInfo>& l);
		inline Database::Info::StageInfo* GetStageInfo() { return stage.GetStageInfo(); }
		inline Database::Info::StageGroup* GetStageGroupInfo() { return stage.GetStageGroupInfo(); }
		inline uint8 GetDifficulty(){ return logic.GetEventListener().GetCurrentStageLicense().Difficulty; }
		bool IsAbleToBuy(Player* player, Constants::ShopType shopType);
		bool IsAbleToTrade(Player* player, Serial target);
		void CheckAbleToPlay();
		inline bool IsUpdateMISC() const { return bUpdateMISC; }
		inline bool IsUpdateNPC() const { return bUpdateNPC; }
		bool GetRespawnPos(uint8, float3&);
		inline bool IsSquare() const { return bSquare; }
		inline float GetAirDamage() const { return airDamage; }
		inline float GetAirDamageByNpc() const { return airDamageByNpc; }

		uint8 GetQuestState(Player* player, uint32 questHash);
		uint8 SetQuestState(Player* player, uint32 questHash, uint8 newState);
		uint32 GetQuestParameter(Player* player, uint32 questHash, uint8 paramIndex);
		uint32 SetQuestParameter(Player* player, uint32 questHash, uint8 paramIndex, uint32 newValue);
		void CompletedQuest(Serial objectSerial, uint32 questHash);
		void AcceptedQuest(Serial objectSerial, uint32 questHash);
		void NoticeHolidayEvent(uint32 eventId, bool start);




		//Event to logic	////////////////////////////////////////////////////////////////////////////////////////////////////
		void Initialized(uint16 uniqueId);
		ILockable& GetSyncRoom();
		bool PlayerCreated(Player* player);
		void NonPlayerCreated(NonPlayerData& data);
		bool Tamed(Player* player, Serial familiar, Constants::Familiar::Type type);
		void StructureCreated(CharacterData& data);
		void VehicleCreated(CharacterData& data);
		void ProjectileCreated(ObjectData& data, const float3& targetPos, Serial target, const std::wstring& owner);
		void ItemCreated(Lunia::XRated::ObjectData& data, const std::wstring& owner, float dt, uint16 stackCount=1, bool isPrivateItem=false);
		void ObjectCreated(ObjectData& data);

		void Died(Player*);
		void Revived(Player* player, bool bSelf=true);


		void ObjectDestroyed(Object* obj, Constants::ObjectType type, bool silent=false);

		bool AddItem(Player* player, uint32 id, int cnt);
		bool AddItem(Player* player, int* arrayId, int* arrayCnt);
		bool RemoveItem(Player* player, uint32 id, int cnt);
		bool RemoveItem(Player* player, int* arrayId, int* arrayCnt);
		bool ExamineItemSlotFromPlayer(uint32 serial, uint32 itemId, int cnt);

		//Pvp Item
		void AcquirePvpItem(Player* player, uint32 itemHash);
		void UsePvpItem(Player* player, uint32 itemHash);
		void DropPvpItem(Player* player);

		void AnimationChanged(Serial s, uint32 ani, const float3& pos, const float3& dir, const float p);
		void CollisionStateChanged(Serial serial, bool col, const float3& position);
		void StatusChanged(Serial serial, float3 pos, float3 dir, float hp, float mp, Serial byWhom, uint32 byWhat, unsigned int sFlag=0, uint32 airComboCount = 0);
		void XpGained(Player* player, XRated::Constants::ExpAcquiredType type, uint64 storyXp, int32 pvpXp=0, int32 warXp=0, Serial beKilledNpc=0);
		bool AcquireItem(Player* player, uint32 itemId, Serial itemSerial, uint16 stackCount=1, InstanceEx instance=0);
		void ItemEquipped(bool bEquipped, Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId);
		void EquipementSwapped(bool bequipped, Player* player, const std::vector< EquippedItem>& newEquipments );
		void CashEquipmentSwapped(bool bequipped, Player* player, const std::vector< EquippedItem>& newEquipments );
		void SkillPointChanged(Player* player, uint32 skillGroupHash, int level);
		void AddedSkillPointPlus(Player* player, uint16 currentSkillPoint, uint16 addedSkillPointPlus, uint16 storedSkillPoint);
		void Casted(Serial gameObjectSerial, uint32 skillGroupName, uint8 skillLevel);
		void LevelUp(Player* player, XRated::Constants::ExpType expType, uint16 level, uint32 storedLevel);
		void SkillPointGot(Player* player, int skillPoint);
		void StateChanged(Serial gameObjectSerial, uint32 id, int level, Serial byWhom=0);
		void LocPlayers(bool lock);
		void SetFloor( int floor );
		void CheckStateBundleForAchievement( uint32 stateBundleHash );
		void DisplayTextEvent(Constants::DisplayTextType type, uint16 textId, float param);
		void NoticeTextEvent(Player* player, Constants::DisplayTextType type, uint16 textId, float param);
		void DisplayText(Constants::DisplayTextType type, const std::wstring& message);
		void NoticeText(Player* player, Constants::DisplayTextType type, const std::wstring& message);
		void DisplayTimer(Constants::DisplayTimerType timer, float seconds);
		void MissionClear(bool success, uint8 team);
		void BroadcastEvent(int eventId, const std::wstring& param);
		void NoticeEvent(Player* player, int eventId, const std::wstring& param);
		void NoticeClientStageEvent( Player* player, int eventId, int eventParam );
		void TeamChange(Serial object, uint8 team);
		void AddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type);
		void NoticeAddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type, Player* player);
		void RemoveMinimapPing(int pingId);
		void GoldGained(Player* player, Constants::GoldRewardAcquiredType type, uint32 amount);


		void GiveLicense(Player* player, uint32 licenseGroup, uint32 accessLevel);
		void GiveCharacterLicense(Player* player, uint32 characterId);

		void PlayerDie(Serial objectSerial, Serial fromSerial);
		void PlayerSave(Serial objectSerial, Serial fromSerial);

		void StyplePointType(Serial objectSerial, XRated::Constants::StylePointType type);

		uint32 GetStageCnt() const { return stageCnt; }
		uint32 GetNextObjectSerial();
		const Stage& GetStage() const { return stage; }

		bool Rebirthed(Player& player);
		void RebirthRollBacked(Player& player);

		void NoticeStylePointState(XRated::StylePoint::State state);

		void ChangeWeatherToNone();
		void ChangeWeatherToClear();
		void ChangeWeatherToRain();
		void ChangeWeatherToSnow();
		void ChangeWeatherToAqua();

		/* Gamble */
		void GambleGameEnd(const std::vector<uint8>& rankList);
		void GambleNextGame(Player* player, float seconds);
		void GambleNextGame(float seconds);
		int GetSlimeRaceIntervalInMin() const;

		/* Holiday Event */
		bool IsHolidayEventTime(uint32 eventId);
		uint32 GetHolidayEventNpcHash(uint32 eventId, uint32 oldNpcHash);
		
		void PetRevived( Player* player );
		void PetChange(Player* player, uint32 petItemHash, XRated::GlobalSerial petItemSerial);
		void PetInfoUpdated( Player* player, const XRated::Pet& pet, const XRated::Serial petSerial, const std::wstring& petExpression );
		void PetDeleted(Player* player, XRated::GlobalSerial petSerial);
		void PetCaredBySchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet );
		void PetTakenOutFromSchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet );
		
		void ChangePlayerStatusLimit( Player* player, const std::vector<uint32>& limits );
		void ChangePlayerStatusRate( Player* player, const std::vector<float>& rate );


#ifdef _SINGLE
		void NPCDebugMessage(Serial serial, std::wstring msg);
		void ChangeNPCDebugMessage(int type);
		void Reload();
#endif

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual void Update(float dt);

	};

}	}	}
