#pragma once
#include "AI/Trigger.h"
#include "Sector/SectorManager.h"
#include <Core/CriticalSection.h>
#include <Info/Info/StageGroup/StageGroupManager.h>
#include <queue>

namespace Lunia { namespace XRated {
	namespace Database { 
		namespace Info {
			struct StageInfo;
		}
		namespace Enchant {
			struct EnchantBitfields;
		}
	}

	namespace Logic {

		class Stage;

	namespace Map {
		class StageMap;
	}

	typedef std::map<Serial, Actor*> ObjectMap;
	typedef std::map<Serial, NonPlayer*> NPCMap;
	typedef std::map<Serial, Player*> PlayerMap;

	class IdleSerialQueue
	{
		Serial             start;
		Serial             current;
		Serial             end;
		std::queue<Serial> extraQueue;

	public:
		inline IdleSerialQueue( Serial startSerial, Serial endSerial )
			: start( startSerial ), current( startSerial ), end( endSerial )
		{
		}

		inline Serial GetSerial( void )
		{
			Serial returnValue = 0;

			if( extraQueue.empty() == false )
			{
				returnValue = extraQueue.front();
				extraQueue.pop();
			}
			else
			{
				if( current == end )
				{
					throw Lunia::Exception(L"Cannot Create More Serial");
				}

				returnValue = current++;
			}

			return returnValue;
		}

		inline void PushExtraSerial( Serial serial )
		{
			extraQueue.push( serial );
		}

		inline void Clear()
		{
			while( extraQueue.empty() == false )
			{
				extraQueue.pop();
			}

                        current = start;
		}		
	};

	class IGameStateDatabase
	{
	public :
		virtual ~IGameStateDatabase() {}

		virtual Object* Remove(Serial)=0;
        //virtual void Remove(Object*)=0;
		virtual Object* Retrieve(Serial)=0;
		virtual Object* RetrieveByID(uint32)=0;
		virtual Actor* RetrieveActor(Serial)=0;
		virtual Actor* RetrieveActorByID(uint32)=0;
		virtual NonPlayer* RemoveNpc(Serial)=0;
		virtual NonPlayer* RetrieveNPC(Serial)=0;
		virtual NonPlayer* RetrieveNPCByID(uint32)=0;
		virtual Player* RemovePlayer(Serial)=0;
		virtual Player* RetrievePlayer(Serial)=0;
		//virtual Player* RetrievePlayerByID(uint32)=0;
		virtual Player* NextPlayer(Serial)=0;
		virtual Player* GetRandomPlayer() =0;

		virtual bool AddToObjectBuffer(uint32 stageCnt, Actor*)=0;
		virtual Serial CreateObject(Actor*)=0;
		virtual bool AddToNPCBuffer(uint32 stageCnt, NonPlayer*)=0;
		virtual Serial CreateNonPlayer(NonPlayer*)=0;
		virtual Serial CreatePlayer(Player*)=0;
		virtual Serial CreatePlayer(Player*, Serial)=0;
		virtual void CreateProjectile(uint32 name, float2 pos, uint16 fireAngle, uint16 type, Object* obj, float min, float max, uint16 toMyTarget, float createTime, Lunia::XRated::Serial creator)=0;
		virtual Serial CreateItem(uint32 name, const float3& pos, const float3& direction, uint16 stackCount=1, InstanceEx instance=0, bool isPrivateItem=false)=0;
		virtual Serial CreateItem(uint32 name, const float3& pos, const float3& direction, uint32 userSerial, uint16 stackCount=1, InstanceEx instance=0, bool isPrivateItem=false)=0;
		virtual void ClearItem()=0;

		virtual void Pause(float time)=0;
		virtual void CampfireBegin()=0;
		virtual void FastMatch()=0;

		virtual AI::TriggerID AddTrigger(uint16 priority, AI::Trigger::TriggerType triggerType, Serial creator,
											Constants::ObjectType objType, float3 pos, float rad, float timeLimit,
											bool bUpdate, bool bImmediately = true)=0;

		virtual void GetAreaObjectRect(const float4& rect, std::vector<Object*>& l)=0;
		virtual void GetAreaObjectRect(const float4& rect, std::vector<Actor*>& l)=0;
		virtual void GetAreaObjectRect(const float4& rect, std::vector<NonPlayer*>& l)=0;
		virtual void GetAreaObjectRect(const float4& rect, std::vector<Player*>& l)=0;
		virtual void GetAreaPlayerByTeam(uint8 team, float3 targetPos, float range, std::vector<Player*>& l)=0;///< ����ġ �й��

		//Retrieve object list
		/** GetAreaObjectForSector9.
			2007.10.23
			auther kwind.	
		*/
		virtual void GetAreaObjectForSector9(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l) =0;
		virtual void GetAreaObject(const float3& targetPos, float range, Object* object, std::vector<Object*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Projectile*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Object* object, std::vector<Object*>& l)=0;

		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)=0;
		
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Object* object, std::vector<Object*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float length, float width, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)=0;
		
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Object* object, std::vector<Object*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Object*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Actor*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<NonPlayer*>& l)=0;
		virtual void GetAreaObject(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, Constants::AllianceType by, uint8 value, Object* object, std::vector<Player*>& l)=0;

		virtual const PlayerMap& GetPlayers()=0;
		virtual const NPCMap& GetNPCs()=0;
		virtual float3 GetAvailablePos(const float3& pos, const float& rad)=0;
		virtual float3 GetAvailablePos(const float3& pos, const float& rad, Actor* it)=0;
		virtual float3 GetAvailableGhostPos(const float3& pos, const float& rad)=0;
		virtual float GetRandomFloat()=0;
		virtual int GetRandomInt(int min, int max)=0;
		virtual Lunia::Math::PseudoRandomFloat* GetRandomClass()=0;
		virtual bool IsAbletoLocate(Serial serial, const float3& pos, float rad)=0;
		virtual bool IsAbletoLocate(Serial serial, const float3& pos, float rad, Actor* it)=0;
		virtual bool IsAbletoLocateInMap(const float3& pos, float rad)=0;
		virtual bool IsAbletoLocateByGhost(const float3& pos, float rad)=0;

		virtual void ObjectMoved(Object*)=0;

		//npc
		virtual void NPCArrived(NonPlayer* npc, int pathCnt, int posCnt)=0;

		virtual Map::StageMap* GetStageMap()=0;
		virtual Constants::GameTypes GetGameType()=0;
		virtual SectorManager& GetSmallSector()=0;
		virtual SectorManager& GetBigSector()=0;
		virtual int GetPlayerCnt() const=0;
		virtual int GetMaxTeamPlayerCnt() const=0;
		virtual bool IsPlayerTeam( uint8 team ) const=0;
		virtual int GetActivePlayerCnt() const=0;
		virtual int GetNPCCnt() const=0;
		virtual int GetMISCCnt() const=0;
		virtual int GetProjectileCnt() const=0;
		virtual Database::Info::StageInfo* GetStageInfo()=0;
		virtual	Database::Info::StageGroup* GetStageGroupInfo() = 0;
		virtual uint8 GetDifficulty()=0;
		virtual const Stage& GetStage() const=0;
		virtual bool IsUpdateMISC() const=0;
		virtual bool IsUpdateNPC() const=0;
		virtual bool GetRespawnPos(uint8, float3&)=0;
		virtual bool IsSquare() const=0;
		virtual float GetAirDamage() const=0;
		virtual float GetAirDamageByNpc() const=0;

		//Event to logic
		virtual void Initialized(uint16 uniqueId)=0;
		virtual std::mutex& GetSyncRoom()=0;
		virtual bool PlayerCreated(Player* player)=0;
		virtual void NonPlayerCreated(NonPlayerData& data)=0;
		virtual bool Tamed(Player* player, Serial familiar, Constants::Familiar::Type type)=0;
		virtual void StructureCreated(CharacterData& data)=0;
		virtual void VehicleCreated(CharacterData& data)=0;
		virtual void ProjectileCreated(ObjectData& data, const float3& targetPos, Serial target, const std::wstring& owner)=0;
		virtual void ItemCreated(Lunia::XRated::ObjectData& data, const std::wstring& owner, float dt, uint16 stackCount, bool isPrivateItem)=0;
		virtual void ObjectCreated(ObjectData& data)=0;
		virtual void Died(Player* player)=0;
		virtual void Revived(Player* player,bool bSelf=true)=0;
		virtual void ObjectDestroyed(Object*, Constants::ObjectType type, bool silent=false)=0;
		virtual bool AddItem(Player* player, uint32 id, int cnt)=0;
		virtual bool AddItem(Player* player, int* arrayId, int* arrayCnt)=0;
		virtual bool RemoveItem(Player* player, uint32 id, int cnt)=0;
		virtual bool RemoveItem(Player* player, int* arrayId, int* arrayCnt)=0;
		virtual bool ExamineItemSlotFromPlayer(uint32 serial, uint32 itemId, int cnt)=0;
		virtual void AcquirePvpItem(Player* player, uint32 itemHash)=0;
		virtual void UsePvpItem(Player* player, uint32 itemHash)=0;
		virtual void DropPvpItem(Player* player)=0;
		virtual void AnimationChanged(Serial s, uint32 ani, const float3& pos, const float3& dir, const float p)=0;
		virtual void CollisionStateChanged(Serial serial, bool col, const float3& position)=0;
		virtual void StatusChanged(Serial serial, float3 pos, float3 dir, float hp, float mp, Serial byWhom, uint32 byWhat, unsigned int sFlag=0, uint32 airComboCount = 0)=0;
		virtual void XpGained(Player* player, XRated::Constants::ExpAcquiredType type, uint64 storyXp, int32 pvpXp=0, int32 warXp=0, Serial beKilledNpc=0)=0;
		virtual bool AcquireItem(Player* player, uint32 itemId, Serial itemSerial, uint16 stackCount, InstanceEx instance)=0;
		virtual void ItemEquipped(bool bEquipped, Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId)=0;
		virtual void SkillPointChanged(Player* player, uint32 skillGroupHash, int level)=0;
		virtual void AddedSkillPointPlus(Player* player, uint16 currentSkillPoint, uint16 addedSkillPointPlus, uint16 storedSkillPoint) =0;
		virtual void Casted(Serial gameObjectSerial, uint32 skillGroupName, uint8 skillLevel)=0;
		//virtual void ItemUsed(Player* player, Database::Info::ItemInfo* info)=0;
		virtual void LevelUp(Player* player, XRated::Constants::ExpType expType, uint16 level, uint32 storedLevel)=0;
		virtual void SkillPointGot(Player* player, int skillPoint)=0;
		virtual void StateChanged(Serial gameObjectSerial, uint32 id, int level, Serial byWhom=0)=0;
		virtual void LocPlayers(bool lock)=0;
		virtual void DisplayTextEvent(Constants::DisplayTextType type, uint16 textId, float param)=0;
		virtual void NoticeTextEvent(Player* player, Constants::DisplayTextType type, uint16 textId, float param)=0;
		virtual void DisplayText(Constants::DisplayTextType type, const std::wstring& message)=0;
		virtual void NoticeText(Player* player, Constants::DisplayTextType type, const std::wstring& message)=0;
		virtual void DisplayTimer(Constants::DisplayTimerType timer, float seconds)=0;
		virtual void MissionClear(bool success, uint8 team)=0;
		virtual void BroadcastEvent(int eventId, const std::wstring& param)=0;
		virtual void NoticeEvent(Player* player, int eventId, const std::wstring& param)=0;
		virtual void NoticeClientStageEvent( Player* player, int eventId, int eventParam ) = 0;
		virtual void TeamChange(Serial object, uint8 team)=0;
		virtual void AddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type)=0;
		virtual void NoticeAddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type, Player* player)=0;
		virtual void RemoveMinimapPing(int pingId)=0;
		virtual void GoldGained(Player* player, Constants::GoldRewardAcquiredType type, uint32 amount)=0;

		virtual void GiveLicense(Player* player, uint32 licenseGroup, uint32 accessLevel)=0;
		virtual void GiveCharacterLicense(Player* player, uint32 characterId)=0;
		virtual int	 GetStageProprietyLevel()=0;

		virtual	void StyplePointType(Serial objectSerial, XRated::Constants::StylePointType type) = 0;
	
		///	For PvP
		virtual	void PlayerDie(Serial objectSerial, Serial fromSerial) = 0;
		virtual	void PlayerSave(Serial objectSerial, Serial fromSerial) = 0;
		

		//for object load thread
		virtual uint32 GetStageCnt() const =0;
		virtual uint32 GetNextObjectSerial()=0;

		/// Rebirth
		virtual bool Rebirthed(Player& player) = 0;
		virtual void RebirthRollBacked(Player& player) = 0;

		virtual void CompletedQuest(Serial objectSerial, uint32 questHash) =0;
		virtual void AcceptedQuest(Serial objectSerial, uint32 questHash) = 0;
#ifdef _SINGLE
		int showNPCInfo;
		virtual void NPCDebugMessage(Serial serial, std::wstring msg)=0;

#endif
		virtual void PetRevived( Logic::Player* player ) = 0;
		virtual void PetChange(Logic::Player* player, uint32 petItemHash, XRated::GlobalSerial petItemSerial) = 0;
		virtual void PetDeleted(Logic::Player* player, XRated::GlobalSerial petSerial) =0;
		virtual void PetInfoUpdated( Player* player, const XRated::Pet& pet, const XRated::Serial petSerial, const std::wstring& petExpression ) = 0;
		virtual void PetCaredBySchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet ) = 0;
		virtual void PetTakenOutFromSchool( Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet ) = 0;
	};


} } }
