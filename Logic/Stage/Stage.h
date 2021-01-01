#pragma once
#include <Info/Info.h>
#include "../Map/Map.h"
#include "ScriptEngine.h"
#include <Core/DeltaTimer.h>


namespace Lunia { namespace XRated {	namespace Logic {

	class GameStateDatabase;
	class IGameStateDatabase;
	class Actor;

	typedef std::vector<uint32> Uint32Vector;

	class Stage : public IStageScript {
		struct Place {
			uint32 userSerial;
            float deter;
			Place(uint32 s, float d) : userSerial(s), deter(d) {}

			static bool Comp( Place s, Place t ) { return s.deter >= t.deter; }
		};

		struct Timer
		{
			float time = 0.0f;
			float alarmAt = 0.0f;
			bool bCountdown = false;
			bool bEnabled = false;

			Timer() {}
			Timer(bool cd) : time(0), bCountdown(cd), bEnabled(false), alarmAt(0) {}
		};

		uint32 id; //> stage identity
		Database::Info::StageInfo* info;
		Database::Info::StageGroup* stageGroup;
		std::vector<float3> locationPosition;

		Database::Info::ItemStageInfo::ItemStageInfoData* pvpItemInfo;

		struct InterestLocation {
			bool bActivated;
			std::list<Serial> locationObjects;
			InterestLocation() : bActivated(false) {}
		};
		std::vector<InterestLocation> interestLocations;


		struct InterestObjectLocation {
			uint32 serial;
			float range;
			bool bRemove; 
			std::list<Serial> objects;
			InterestObjectLocation(uint32 s, float r) : serial(s), range(r), bRemove(false) {}
		};
		std::list<InterestObjectLocation> interestObjectLocations;

		struct InterestObjects{
			// type : Player=0, NonPlayer=1, Structure=2, Projectile=3, Vehicle=4, Misc=5, Item=6, TombStone=7
			const static uint16 Player		= 0x0001;
			const static uint16 NonPlayer	= 0x0002;
			const static uint16 Structure	= 0x0004;
			const static uint16 Projectile	= 0x0008;
			const static uint16 Vehicle		= 0x0010;
			const static uint16 Misc		= 0x0020;
			const static uint16 Item		= 0x0040;
			const static uint16 TombStone	= 0x0080;

			Uint32Vector destroy;
			Uint32Vector pathFinding;
			Uint32Vector serials;

			InterestObjects() { destroy.reserve(20); pathFinding.reserve(10); serials.reserve(10); }
			void Clear() { destroy.clear(); pathFinding.clear(); serials.clear(); }
			bool IsInterestObject(Serial serial);
			bool IsInterestObjectID(uint32 hash);
			void SetInterestNPCPath(uint32 hash, bool bAdd);
			void SetInterestObjectDestroy(uint32 hash, bool bAdd);
		} interestObjects;

		struct GameState {
			const static int NotInitializedWithoutScript = -2;
			const static int WithoutScript = -1;
			const static int NotInitialized = 0;
			const static int MissionStarted = 1;
			const static int CoinPlayerDied = 2;
			const static int ContinueWait = 3;
			const static int MissionEnded = 4;
			const static int CampfireWait = 5;
			const static int Campfire = 6;
			const static int ContinueCountDown = 7;
		};

		ScriptEngine script;

		Map::StageMap stageMap;

		GameStateDatabase* gameStateDatabase;

		Lunia::DeltaTimer<float> clock;
		std::vector<Timer> timers;

		struct ContinueTimer{
			float	ContinueTime;
			bool	IsEnable;

			void Reset()
			{

				ContinueTime = 6.0f;
			}

		} continueTimer;		

		int maxNpcCnt;
		Constants::GhostRules::Type ghostRule;
		Constants::GameTypes gameType;
		bool bFastMatchActivated;

		float campfireDelay;

		float pvpMissionClearDelay;
		void SetPvpMissionClearDelay( float delay ) { pvpMissionClearDelay= delay; }



		int gameState;
		bool bObjectDestroying;

		//StageStatistics
		float MissionPlayTime;
		int secretCnt;

		struct Coin {
			std::map<uint32 /*player serial*/, float /*cooldown*/> continueDelay;
			uint8 team;
			bool success;
		} coin;

	public :
		Stage();
		virtual ~Stage();

		inline void SetDatabase(GameStateDatabase* stage) { gameStateDatabase = stage; }
		inline Map::StageMap* GetStageMap() { return &stageMap; }
		inline Database::Info::StageInfo* GetStageInfo() { return info; }
		inline Database::Info::StageGroup* GetStageGroupInfo() const { return stageGroup; }

	public :
		void Init();
		inline bool IsLoading() const { return script.IsLoading(); }
		void LoadStage(Database::Info::StageGroup* sGroup, uint32 name, uint16 uniqueId);
		inline bool IsAbletoLocate(const float3& pos, float rad, Map::StageMap::Height height) { return stageMap.IsAbletoLocate(pos, rad, height); }
		bool IsAbletoBuy(const float3& pos, Constants::ShopType shopType);
		inline const float2& GetMapSize() const { return stageMap.GetLogicalSize(); }
		inline float3 GetStartPosition() const { return info->PlayerStartingPoints[0][0].Position; }
		inline Constants::GhostRules::Type GetGhostRule() const { return ghostRule; }
		inline void SetGameType( Constants::GameTypes type) { gameType = type; }
		inline Constants::GameTypes GetGameType() const { return gameType; }
		inline bool IsFastMatchActivated() const { return bFastMatchActivated; }
		void GetStageStatistics(CriticalStatistics::StageStatistics& SS);
		inline void ResetMissionPlayTime() { MissionPlayTime = 0; }
		inline bool IsMissionCleared() const {
			switch (gameState) {
			case GameState::MissionEnded :
			case GameState::CampfireWait :
			case GameState::Campfire :
				return true;
			default :
				return false;
			}
		}
		void CoinPlayerDied(uint32 serial);
		//void CoinPlayerPart(uint32 serial);
		void CoinPlayerRevived(uint32 serial);
		void CoinPlayerGiveUp(uint32 serial);

		void Update();

		float GetElpasedTime();

	private :
		void UpdateObjectLocation();
		void UpdateLayer();
		void UpdateTimer(float dt);
		bool UpdateContinueTimer(float dt);
		bool UpdateCoinPlayerCountdown(float dt);

		void CreatePvpItemInPosition(std::vector<Place>& l);
		void CreatePvpItem(std::vector<Place>& l); 

	public :// Event to script engine.
		void ObjectDestroyed(Serial serial, uint32 hash, Constants::ObjectType type);
		void NPCArrived(uint32 hash, uint32 serial, int pathCnt, int posCnt);
		void InterestObjectDestroyed(Serial serial);
		void PvpGameStart();
		void PlayerDied(Serial serial, uint16 life);
		void CompletedQuest(Serial serial, uint32 questHash);
		void AcceptedQuest(Serial serial, uint32 questHash);
		void NoticeHolidayEvent(uint32 eventId, bool start);

	public :// IStageScript impl	////////////////////////////////////////////////////////////////////////////////////////
		uint32 GetStageId() const { return id; }

		std::mutex& GetSyncRoom();
		void Initialized(uint16 uniqueId);
		bool IsPlayer(uint32 userSerial);
		int GetPlayerCnt() const;
		int GetActivePlayerCnt() const;
		uint8 GetPlayerTeam(uint32 userSerial) const;
		int GetPlayerClass(uint32 userSerial) const;
		int	GetRandomInt(int min, int max);
		void GetNPCPosition(Serial serial, float& x, float& y);
		int GetPlayerLv(uint32 userSerial) const;

		// Addby Caspian 20071115
		uint16 GetPlayerCnt(uint8 team)  const;
		int GetPlayerSerial( uint8 team, uint16 index ) const ;

		uint16 GetPlayablePlayerCnt() const;
		uint16 GetPlayablePlayerCnt(uint8 team) const;
		bool IsPlayable( uint32 playerSerial ) const;

		void PlayerLeft( Serial playerSerial, uint8 team ) { script.PlayerLeft( playerSerial, team ); }
		void PlayerEntered( Serial playerSerial, uint8 team ) { script.PlayerEntered( playerSerial, team); }

		///* stage */
		void SetPvpItemInfo(uint32 hash);
		void DropPvpItem();
		void AddInterestLocation(int location, bool bAdd);
		bool IsFulfilledInterestLocation( int location );
		void AddInterestObjectLocation(uint32 npcSerial, float range, bool bAdd);
		void AddInterestObjects(uint32 hash, bool bAdd);
		void AddInterestNPCPath(uint32 hash, bool bAdd);
		void AddInterestObjects(uint32 serial);

		void Pause(float time);
		void LocPlayers(bool lock);
		void TimerRun(int id, float alarmAt);
		void TimerPause(int id);
		int SecretFound() { return ++secretCnt; }
		void MissionClear(bool success, uint8 team);
		void MissionClearFromLogic(bool success, uint8 team);
		void MissionClearTimeOver();
		inline void ActivateFastMatch() { bFastMatchActivated = true; }
		void ContinueCountDown();
		void SetFloor( int floor );

		///* object */
		void ClearItem();
		void CreateItem(uint32 id, const float3& pos, uint32 userSerial, const float3& direction);
		void CreateItem(uint32 id, int location, int cnt, uint32 userSerial = 0);
		void CreateItem(uint32 id, const float3& pos, uint32 userSerial = 0, bool isPrivateItem = false);
		uint32 CreateObject(uint32 id, float2 pos, uint8 team);
		uint32 CreateObject(uint32 id, float2 pos, const float3& direction, uint8 team);
		uint32 CreateNPC(uint32 id, float3 pos, uint8 team, Database::Info::StageInfo::Npc::IdleStateType idleType,
						int pathGroupIndex, int defenceLocation, Database::Info::StageInfo::Npc::TransferType transferType);
		uint32 CreateNPC(uint32 id, float3 pos, const float3& direction, uint8 team, Database::Info::StageInfo::Npc::IdleStateType idleType,
						int pathGroupIndex, int defenceLocation, Database::Info::StageInfo::Npc::TransferType transferType);
		void DestroyObject(uint32 id, bool silent);
		void DestroyObject(const int& location, uint32 id, bool silent);
		void DestroyObject(const int& location, const uint8& team, bool silent); 
		void DestroyNPC(const int& location, uint32 id, bool silent); 
		void DestroyNPC(const int& location, const uint8& team, bool silent); 
		void ChangePlayerTeam(uint32 userSerial, uint8 team);
		void ChangeNpcTeam(uint32 serial, uint8 team);
		void ChangeNpcAI(int location, uint32 id, int idleType, int value);
		void ChangeNpcAI(uint32 id, int idleType, int value);
		template<class T> void MoveObject(int locationFrom, int locationTo);
		template<class T> void MoveObject(int locationFrom, int locationTo, uint32 id);
		void MoveObject(int locationFrom, int locationTo, uint16 objectType);
		void MoveObject(int locationFrom, int locationTo, uint32 team, uint16 objectType);
		void MovePlayer(uint32 serial, int location);
		void MovePlayer(int location);
		void ChangeMISCState(uint32 id, uint32 type, bool bAdd);
		void ChangeMISCState(uint32 id, int location, uint32 type, bool bAdd);
		void ChangeNPCState(uint32 id, uint32 type, bool bAdd);
		void ChangeNPCState(uint32 id, int location, uint32 type, bool bAdd);

		uint32 CheckMinimumPlayerCount();
		void CheckStateBundleForAchievement( uint32 stateBundleHash );
		void GiveStateBundleToObject( uint32 objectSerial, uint32 id );

		///* object status */
		void ChangeNpcSight(uint32 serial, float range);
		void GiveExp(XRated::Constants::ExpAcquiredType type, uint32 userSerial, uint16 level, uint32 exp);
		void GiveExp(XRated::Constants::ExpAcquiredType type, uint16 level, uint32 exp);
		void GiveBattleEXPtoAllPlayer(XRated::Constants::ExpAcquiredType type, uint8 team, uint32 exp);
		void AddHp(uint32 id, int delta);
		void AddMp(uint32 id, int delta);
		void DealDamage(int location, uint8 team, float dmg, bool bRate);
		void DealDamage(int location, uint32 hash, float dmg, bool bRate);
		void ChangeNPCAction( uint32 serial, uint32 action);
		void ChangeNPCAction(int location, uint32 hash, uint32 action);
		bool AddItemToPlayer(uint32 userSerial, uint32 itemId, int cnt, bool bToTeam);
		bool AddItemToPlayer(uint32 userSerial, int* arrayId, int* arrayCnt);
		bool RemoveItem(uint32 userSerial, uint32 itemId, int cnt, bool bFromTeam);
		bool RemoveItem(uint32 userSerial, int* arrayId, int* arrayCnt);
		bool ExamineItemSlotFromPlayer(uint32 userSerial, uint32 itemId, int cnt);
		void ChangePlayerStatusLimit( std::vector<uint32>* statuses, const std::vector<uint32>& limits);
		void ChangePlayerStatusRate( std::vector<uint32>* statusId, const std::vector<float>& rate);

		///* Quest */
		uint8 GetQuestState(uint32 userSerial, uint32 questHash);
		uint8 SetQuestState(uint32 userSerial, uint32 questHash, uint8 newState);
		uint32 GetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex);
		uint32 SetQuestParameter(uint32 userSerial, uint32 questHash, uint8 paramIndex, uint32 newValue); 

		///* output */
		void DisplayTextEvent(Constants::DisplayTextType displayTo, uint16 textId, float param);
		void NoticeTextEvent(uint32 userSerial, Constants::DisplayTextType displayTo, uint16 textId, float param);
		void DisplayText(Constants::DisplayTextType displayTo, const std::wstring& text);
		void NoticeText(uint32 userSerial, Constants::DisplayTextType displayTo, const std::wstring& text);
		void DisplayTimer(int timer, Constants::DisplayTimerType type);
		void BroadcastEvent(int eventId); // contains cinematic
		void NoticeEvent(uint32 userSerial, int eventId); // personal event
		void NoticeClientStageEvent( uint32 userSerial, int eventId, int eventParam );
		void AddMinimapPing(int pingId, int x, int y, Constants::MiniMapPingType type);
		void NoticeAddMinimapPing(int pingId, int x, int y, Constants::MiniMapPingType type, uint32 userSerial);
		void RemoveMinimapPing(int pingId);

		void GiveLicense(uint32 LicenseGroup, uint32 accessLevel);
		void GiveLicenseToPlayer(uint32 userSerial, uint32 LicenseGroup, uint32 accessLevel);
		void GiveCharacterLicense(uint32 userSerial, uint32 characterId);

		void NoticeStylePointState(XRated::StylePoint::State state);

		/* Gamble */
		void GambleGameEnd(std::vector<uint8> rankList);
		void GambleNextGame(uint32 userSerial, float seconds);
		int GetSlimeRaceIntervalInMin() const;

		/* Holiday Event */
		bool IsHolidayEventTime(uint32 eventId);
		uint32 GetHolidayEventNpcHash(uint32 eventId, uint32 oldNpcHash);

		void ChangeWeatherToNone();
		void ChangeWeatherToClear();
		void ChangeWeatherToRain();
		void ChangeWeatherToSnow();
		void ChangeWeatherToAqua();

#ifdef _SINGLE
	public : //Single Help functions
		int GetInterestObjectCnt() const { return (int)interestObjects.destroy.size(); }
		int GetInterestSerialCnt() const { return (int)interestObjects.serials.size(); }
		int GetInterestLocationCnt() const {
			int returnValue = 0;
			int cnt = (int)interestLocations.size();
			while ( cnt ) {
				--cnt;
				if ( interestLocations[cnt].bActivated )
					++returnValue;
			}
			return returnValue;
		}
		//reverse hash table�� ������������ �ϴ� ���� �Ѵ�.
		//void GetInterestObjectList( std::vector<uint32>& l) {
		//	Uint32Vector::iterator i = interestObjects.destroy.begin();
		//	while( i != interestObjects.destroy.end() ) {
		//		l.push_back( (*i) );
		//		++i;
		//	}
		//}
#endif
	};

}	}	}
