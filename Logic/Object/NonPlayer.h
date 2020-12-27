#pragma once
#include "Actor.h"
#include "../Status/StatusCharacter.h"
#include "../AI/FSM.h"
#include "../AI/Trigger.h"

//#include "Lunia/XRated/XRatedUI/GUIManager.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class NonPlayer : public Actor, public AI::FSM {
	private :
		const static float EXPRANGE;
		const static float ReselectTargetInterval;

	protected :
		NonPlayerData npcData;

		StatusNonPlayer NPCStatus;

		bool bDead;
		float atkMultiply;
		float hitIgnoreMultiply;
		bool  attackedByPlayer;

	public :

		// NonPlayer Type
		// 일반형, 스톱형, 도망형
		enum NonPlayerType{
			Normal = 0, Stop, Away, NonPlayerTypeCount
		};
		
		int	  trycount;
		float thinktime;
		float timeToNextReselectTarget;
		int Think(); // 현제 행동을 계속 할건지에 대해 생각한다.

	protected :
		void DistributeXp(Object* who);

	public :
		NonPlayer();
		virtual ~NonPlayer() {}

		virtual int GetClass() const { return (int)NPCStatus.npcInfo->Class; } //Actor.h 를 참조.

		virtual void LoadStatus( IGameStateDatabase* db, Database::Info::NonPlayerInfo* info );
		virtual void Initialize(Lunia::Math::RandomInt& rInt);
		virtual void SetAIInfo(const Database::Info::StageInfo::Npc::IdleStateType&, Database::Info::PathGroup*, const float4&, const float3&, const Database::Info::StageInfo::Npc::TransferType&, Serial owner);

		void ModifyNpcInfoByPlayerCnt(float hpModifier, float xpModifier, float atkMultiplyModifier, float hitIgnoreMultiplyModifier);
		void ModifyNpcInfoByRaidStageDiffy(unsigned int lv, const std::wstring& basicStat, uint32 xp);
		void ModifyNpcInfo( float hpFactor , float xpFactor , float intFactor , float dmgFactor, float updateNormalTick=0.5f, float updateFightTick=0.5f );

		Lunia::XRated::NonPlayerData& GetNonPlayerData() { return npcData; }
		Database::Info::NonPlayerInfo* GetNPCInfo() const { return NPCStatus.npcInfo; }

		void SetSight(float s) { NPCStatus.sight = s; }

		//IObject Impl
		virtual const std::wstring& GetName() const { return NPCStatus.npcInfo->Name; }
		virtual void SetName(const std::wstring& name) { NPCStatus.npcInfo->Name = npcData.BaseCharacter.BaseObject.Name = name; }
		virtual void SetPosition(const float3& pos);

		const std::wstring& GetTemplateName() const { return NPCStatus.npcInfo->TemplateName; }
		NonPlayerData::NpcType GetNPCType() { return NPCStatus.npcInfo->NpcType; }
		std::vector<Database::Info::NonPlayerInfo::Item>& GetItemList() { return NPCStatus.npcInfo->Items; }

		void SetHP(float hp, float max=0);
		virtual void ChangeHpFromState(float hp, float max,Lunia::XRated::Serial donor);
		virtual bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);
		virtual bool Update(float dt, IGameStateDatabase* db);
		
		virtual	Serial	GetOwnerSerial() const { return NPCStatus.familiar; }

		bool IsCharacter() const { return true; }
		bool IsDead() const { return bDead; }
		bool IsDropEventNPC();

		//소환수 명령
		void MasterDestroyed(bool bSacrifice) { MasterCommandMeSuicide(bSacrifice); NPCStatus.familiarPtr = NULL; }
		void MasterCommandMeAttack(const float3& pos);
		void MasterCommandMeGuard();
		void MasterCommandMeCome(const float3& pos);
		void MasterCommandMeSuicide(bool bSacrifice);
		void MasterCommand(uint32 actionHash, bool bForce);
		void MasterPraiseMe(float hpAmount);


		//생성할 당시의 파티원 숫자
		void SetPartyCntWhenCreated(int cnt);

		const float GetAtkMultiply() const { return atkMultiply; }
		float GetIncreasedDamageRate(int str);

	public :
		uint64 GetTriggerFlag() { return triggerFlag; }
		virtual bool HandleTrigger(AI::Trigger& /*trigger*/);

		Serial GetTarget() const { return NPCStatus.target; }
		bool IsPetType() const { return ( NPCStatus.idleType == Database::Info::StageInfo::Npc::IdleStateType::PET ); }
		////////////////////////////////////////////////////////////////////////////////////////
		//Hate functions
		Lunia::XRated::Serial GetMostHate() { return NPCStatus.target; }
		void ChangeHate(Lunia::XRated::Serial serial, float hate);
		void ReduceHate(float hate);
		void RemoveHateTarget(Lunia::XRated::Serial serial);
		void ClearHateList() { NPCStatus.hateList.clear(); NPCStatus.target = 0; }
		virtual Lunia::XRated::Serial RecalculateMostHateTarget();
		virtual void Taunted (Lunia::XRated::Serial serial, float duration);	//언놈인가가 taunt를 사용했다.

#ifdef _SINGLE //싱글에서 npc hate list정보를 보여주기 위한 함수
		virtual void GetHateList( std::map<Serial, float>& l ) { l = NPCStatus.hateList; }
#endif
		////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation
	protected :
		AI::FSMState<NonPlayer>* stateIdle;
		AI::FSMState<NonPlayer>* stateApproachTarget;
		AI::FSMState<NonPlayer>* stateFightPtr;

		AI::FSMState<NonPlayer> stateIdleNone;
		AI::FSMState<NonPlayer> stateIdlePathFinding;
		AI::FSMState<NonPlayer> stateIdleDefence;
		AI::FSMState<NonPlayer> stateIdleSoldierOfFortune;

		AI::FSMState<NonPlayer> stateApproachTarget_Sight;		///< For Transfer state by Sight
		AI::FSMState<NonPlayer> stateApproachTarget_HateList;	///< For transfer state by hatelist
		AI::FSMState<NonPlayer> stateCaution;
		AI::FSMState<NonPlayer> stateFight;
		AI::FSMState<NonPlayer> stateWarning;
		AI::FSMState<NonPlayer> stateRetreat;
		AI::FSMState<NonPlayer> stateEndOperation;
		AI::FSMState<NonPlayer> stateCooperation;

		//소환수 AI
		AI::FSMState<NonPlayer> stateFamiliarSummoned; //소환수가 소환되어 바로 수행하는 ai. 기본액션이 끝나면 다음 ai로 넘어간다.
		AI::FSMState<NonPlayer> stateFamiliarAttackGround;
		//AI::FSMState<NonPlayer> stateFamiliarApproachTarget;
		AI::FSMState<NonPlayer> stateFamiliarFight;

		NonPlayerType playertype;

		uint64 triggerFlag;	///< 자신이 관심있는 event trigger 를 설정.

		//For IdleNone State
		float3 initialPos;

		float3 targetPosition;	//목표 위치, idle 상태에서는 목표 위치, 전투중일 때는 자신이 idle상태에서 있었던 최종위치가 저장된다.
		uint32 posCnt;

		//두개의 방향은 자신이 가야할 방향, 네개의 방향은 자신의 패스가 막혔을 때 가야할 방향
		uint32 targetDirection[6];
		int currentDirection;//위에서 현재 가야할 방향
		int collisionCnt;// 방향변경시 충돌한 횟수. direction을 막힐때마다 변경하기 위해서

		float agroCheckTime; //일정 시간마다 시야를 검색해서 상대가 있는지 검사.
		float agroClearTime; //일정 시간마다 hatelist를 초기화 해주기 위해서 필요함.
		float idleCheckTime; //일정 시간동안 idle상태에서 목표 지점에 도달하지 못했다면 워프 시켜준다.

		//좌우로 게걸음-_-을 할때 정해진 방향.
		int sideStep;///< 0은 왼쪽 1은 오른쪽. enum 같은거 하기 귀찮타. 이런 짜실한 부분에.

		float updateNormalTick;
		float updateFightTick;

		bool DoAttack(Actor* target);
		virtual Serial SetTargetToNearest();
		virtual Serial SetTargetToMostHate();

	public :
		virtual void Die(Object* who);
	
		bool CheckSurroundEnemy();//Check enemy in sight. return true when enemy founded.
		inline bool IsAbleToRecognize(Actor*);

		virtual void SetIdleState(Database::Info::StageInfo::Npc::IdleStateType type);
		void SetInitialPos(const float3& pos) { initialPos = pos; }
		void SetAIPath(Database::Info::PathGroup* path);// pathfinding
		void SetAIArea(const float4& area);//area defence

		virtual void IdleNoneBegin();
		// NOTE : child class delived from NonPlayer class should call this function from overrided function
		// after or before running implemented codes.
		virtual void IdleNoneUpdate();
		virtual void IdleNoneEnd();

		virtual void IdleDefenceBegin();
		// NOTE : child class delived from NonPlayer class should call this function from overrided function
		// after or before running implemented codes.
		virtual void IdleDefenceUpdate();
		virtual void IdleDefenceEnd();

		virtual void IdlePathFindingBegin();
		// NOTE : child class delived from NonPlayer class should call this function from overrided function
		// after or before running implemented codes.
		virtual void IdlePathFindingUpdate();
		virtual void IdlePathFindingEnd();

		virtual void IdleSoldierOfFortuneBegin();
		// NOTE : child class delived from NonPlayer class should call this function from overrided function
		// after or before running implemented codes.
		virtual void IdleSoldierOfFortuneUpdate();
		virtual void IdleSoldierOfFortuneEnd() {}

		virtual void stateApproachTarget_SightBegin();
		virtual void stateApproachTarget_SightUpdate();
		virtual void stateApproachTarget_SightEnd() {}

		virtual void stateApproachTarget_HateListBegin();
		virtual void stateApproachTarget_HateListUpdate();
		virtual void stateApproachTarget_HateListEnd() {}

		virtual void stateCaution_HateListBegin() {}
		virtual void stateCaution_HateListUpdate();
		virtual void stateCaution_HateListEnd() {}

		virtual void stateCaution_SightBegin() {}
		virtual void stateCaution_SightUpdate();
		virtual void stateCaution_SightEnd() {}

		virtual void WarningBegin();
		// NOTE : child class delived from NonPlayer class should call this function from overrided function
		// after or before running implemented codes.
		virtual void WarningUpdate();
		virtual void WarningEnd();

		virtual void stateRetreat_Begin() {}
		virtual void stateRetreat_Update() {}
		virtual void stateRetreat_End() {}

		virtual void FightBegin();
		virtual void FightUpdate();
		virtual void FightEnd() {}

		virtual void EndOperationBegin() {}
		virtual void EndOperationUpdate() {}
		virtual void EndOperationEnd() {}

		virtual void CooperationBegin() {}
		virtual void CooperationUpdate() {}
		virtual void CooperationEnd() {}

		//Familiar ai
		void FamiliarSummonedBegin();
		void FamiliarSummonedUpdate();
		void FamiliarSummonedEnd() {}

		void FamiliarAttackGroundBegin();
		void FamiliarAttackGroundUpdate();
		void FamiliarAttackGroundEnd() {}

		void FamiliarFightBegin();
		void FamiliarFightUpdate();
		void FamiliarFightEnd() {}

		void ResetFamiliarStatusByPlayerStat(int Str, int Dex, int Int, int Vit, int rebirthCount=0, bool isFirst=false);

		virtual const float GetHitIgnoreProb( const float hitIgnoreValue ) const;
	};

}	}	}
