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
		// �Ϲ���, ������, ������
		enum NonPlayerType{
			Normal = 0, Stop, Away, NonPlayerTypeCount
		};
		
		int	  trycount;
		float thinktime;
		float timeToNextReselectTarget;
		int Think(); // ���� �ൿ�� ��� �Ұ����� ���� �����Ѵ�.

	protected :
		void DistributeXp(Object* who);

	public :
		NonPlayer();
		virtual ~NonPlayer() {}

		virtual int GetClass() const { return (int)NPCStatus.npcInfo->Class; } //Actor.h �� ����.

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

		//��ȯ�� ���
		void MasterDestroyed(bool bSacrifice) { MasterCommandMeSuicide(bSacrifice); NPCStatus.familiarPtr = NULL; }
		void MasterCommandMeAttack(const float3& pos);
		void MasterCommandMeGuard();
		void MasterCommandMeCome(const float3& pos);
		void MasterCommandMeSuicide(bool bSacrifice);
		void MasterCommand(uint32 actionHash, bool bForce);
		void MasterPraiseMe(float hpAmount);


		//������ ����� ��Ƽ�� ����
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
		virtual void Taunted (Lunia::XRated::Serial serial, float duration);	//����ΰ��� taunt�� ����ߴ�.

#ifdef _SINGLE //�̱ۿ��� npc hate list������ �����ֱ� ���� �Լ�
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

		//��ȯ�� AI
		AI::FSMState<NonPlayer> stateFamiliarSummoned; //��ȯ���� ��ȯ�Ǿ� �ٷ� �����ϴ� ai. �⺻�׼��� ������ ���� ai�� �Ѿ��.
		AI::FSMState<NonPlayer> stateFamiliarAttackGround;
		//AI::FSMState<NonPlayer> stateFamiliarApproachTarget;
		AI::FSMState<NonPlayer> stateFamiliarFight;

		NonPlayerType playertype;

		uint64 triggerFlag;	///< �ڽ��� �����ִ� event trigger �� ����.

		//For IdleNone State
		float3 initialPos;

		float3 targetPosition;	//��ǥ ��ġ, idle ���¿����� ��ǥ ��ġ, �������� ���� �ڽ��� idle���¿��� �־��� ������ġ�� ����ȴ�.
		uint32 posCnt;

		//�ΰ��� ������ �ڽ��� ������ ����, �װ��� ������ �ڽ��� �н��� ������ �� ������ ����
		uint32 targetDirection[6];
		int currentDirection;//������ ���� ������ ����
		int collisionCnt;// ���⺯��� �浹�� Ƚ��. direction�� ���������� �����ϱ� ���ؼ�

		float agroCheckTime; //���� �ð����� �þ߸� �˻��ؼ� ��밡 �ִ��� �˻�.
		float agroClearTime; //���� �ð����� hatelist�� �ʱ�ȭ ���ֱ� ���ؼ� �ʿ���.
		float idleCheckTime; //���� �ð����� idle���¿��� ��ǥ ������ �������� ���ߴٸ� ���� �����ش�.

		//�¿�� �԰���-_-�� �Ҷ� ������ ����.
		int sideStep;///< 0�� ���� 1�� ������. enum ������ �ϱ� ����Ÿ. �̷� ¥���� �κп�.

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
