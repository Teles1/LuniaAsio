#include "NonPlayer.h"
#include "Player.h"
#include <Info/Info/Database.h>
#include "../State/States.h"
#include "../State/StateRegen.h"
#include "../GameState.h"
#include "../AI/AIGlobalFunctions.h"
#include "../LogicDB/LogicDB.h"
#include <Core/DeltaTimer.h>

using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Logic::AI;
using namespace Lunia::Math;


namespace Lunia { namespace XRated {	namespace Logic {

	const float NonPlayer::EXPRANGE = 240.0f;
	const float NonPlayer::ReselectTargetInterval = 10.0f;

	int NonPlayer::Think()
	{
		int ret = -1;

		
		if( thinktime >= 3.0f )
		{
			ret = static_cast< int >( stageData->GetRandomInt( 0, 2 ) );			
			thinktime = 0;
		}

		return ret;

	}

	//////////////////////////////////////////////////////////////////////////////////////

	NonPlayer::NonPlayer()
			: Actor(Lunia::XRated::Constants::ObjectType::NonPlayer), npcData(characterData), currentDirection(0), collisionCnt(0)
	{
		triggerFlag = Trigger::TriggerType::DESTINATION_ARRIVED;
		sideStep = 0;
		//sideStep = sd.GetRandomInt(0, 2); //0�� 1 ���߿� �ϳ��� ����

		//AI FSMState define
		stateIdleNone.Set(L"idle", this, &NonPlayer::IdleNoneBegin, &NonPlayer::IdleNoneUpdate, &NonPlayer::IdleNoneEnd);
		stateIdlePathFinding.Set(L"idle", this, &NonPlayer::IdlePathFindingBegin, &NonPlayer::IdlePathFindingUpdate, &NonPlayer::IdlePathFindingEnd);
		stateIdleDefence.Set(L"idle", this, &NonPlayer::IdleDefenceBegin, &NonPlayer::IdleDefenceUpdate, &NonPlayer::IdleDefenceEnd);
		stateIdleSoldierOfFortune.Set(L"idle", this, &NonPlayer::IdleSoldierOfFortuneBegin, &NonPlayer::IdleSoldierOfFortuneUpdate, &NonPlayer::IdleSoldierOfFortuneEnd);
		stateIdle = &stateIdleNone;

		stateApproachTarget_Sight.Set(L"approachTarget", this, &NonPlayer::stateApproachTarget_SightBegin,
													&NonPlayer::stateApproachTarget_SightUpdate, &NonPlayer::stateApproachTarget_SightEnd);
		stateApproachTarget_HateList.Set(L"approachTarget", this, &NonPlayer::stateApproachTarget_HateListBegin,
													&NonPlayer::stateApproachTarget_HateListUpdate, &NonPlayer::stateApproachTarget_HateListEnd);
		stateApproachTarget = &stateApproachTarget_Sight;
		stateCaution.Set(L"caution", this, &NonPlayer::stateCaution_SightBegin, &NonPlayer::stateCaution_SightUpdate, &NonPlayer::stateCaution_SightEnd);
		stateFight.Set(L"fight", this, &NonPlayer::FightBegin, &NonPlayer::FightUpdate, &NonPlayer::FightEnd);
		stateWarning.Set(L"warning", this, &NonPlayer::WarningBegin, &NonPlayer::WarningUpdate, &NonPlayer::WarningEnd);
		stateRetreat.Set(L"retreat", this, &NonPlayer::stateRetreat_Begin, &NonPlayer::stateRetreat_Update, &NonPlayer::stateRetreat_End);
		stateFamiliarSummoned.Set(L"familiarSummoned", this, &NonPlayer::FamiliarSummonedBegin, &NonPlayer::FamiliarSummonedUpdate, &NonPlayer::FamiliarSummonedEnd);
		stateFamiliarAttackGround.Set(L"FamiliarAttackGround", this, &NonPlayer::FamiliarAttackGroundBegin, &NonPlayer::FamiliarAttackGroundUpdate, &NonPlayer::FamiliarAttackGroundEnd);
		//stateFamiliarApproachTarget.Set(L"FamiliarApproachTarget", this, FamiliarApproachTargetBegin, FamiliarApproachTargetUpdate, FamiliarApproachTargetEnd);
		stateFamiliarFight.Set(L"FamiliarFight", this, &NonPlayer::FamiliarFightBegin, &NonPlayer::FamiliarFightUpdate, &NonPlayer::FamiliarFightEnd);
		stateEndOperation.Set(L"endOperation", this, &NonPlayer::EndOperationBegin, &NonPlayer::EndOperationUpdate, &NonPlayer::EndOperationEnd);
		stateCooperation.Set(L"cooperation", this, &NonPlayer::CooperationBegin, &NonPlayer::CooperationUpdate, &NonPlayer::CooperationEnd);

	}

	void NonPlayer::LoadStatus( IGameStateDatabase* db, Database::Info::NonPlayerInfo* info )
	{
		//if (!NPCStatus.npcInfo) {
		//	throw ( Lunia_EXCEPTION(( L"[NonPlayer::LoadStatus] NPC [%s] information not found.", info->Name.c_str() )) );
		//}

		Actor::Initialize(db);
		
		//NPCStatus.npcInfo = info;
		NPCStatus.Init(info);

		status.Lv = NPCStatus.info->Level;

		objectData.Radius = NPCStatus.npcInfo->Radius;
		objectData.Name = NPCStatus.npcInfo->Name;
		objectData.NameHash = NPCStatus.npcInfo->HashcodeName;
		characterData.Xp = NPCStatus.npcInfo->Exp;
		//npcData.Scale = NPCStatus.npcInfo->Scale;
		npcData.Npc = NPCStatus.npcInfo->NpcType;

		status.Str = NPCStatus.info->Strength;
		status.Dex = NPCStatus.info->Dextrity;
		status.Int = NPCStatus.info->Intelligence;
		status.Vit = NPCStatus.info->Vitality;

		status.dmg.basicStr = NPCStatus.info->Strength;
		status.dmg.basicDex = NPCStatus.info->Dextrity;

		status.basicHp = NPCStatus.info->Hp;
		status.basicMp = NPCStatus.info->Mp;
		status.stackedInt = NPCStatus.info->Intelligence;
		status.stackedVit = NPCStatus.info->Vitality;

		for ( int a=0; a<11; ++a) {
			AddResist((float)NPCStatus.npcInfo->Resists[a], a);
		}
		status.element = Constants::StringToDmgType(info->Element);
		LogicDBInstance().stateDB.AddNPCState( NPCStatus.npcInfo->HashcodeName, this);

		status.dmg.str = status.GetStr();
		status.dmg.dex = status.GetDex();
		RecalculateStatus(true);

		status.Mp = status.MaxMp;
		status.Hp = status.MaxHp;	

		//Pool�̶� �ʱ�ȭ �����ָ� updateTick�� �߸��ɼ� �ִ�. �ʱ�ȭ�ϴµ��� �ָ��ؼ� ����ٰ� ����.
		//Lunia::Config::ConfigInstance().Get(L"stageserver",	L"updateTick", updateTick, 0.5f);
		updateNormalTick = 0.5f;
		updateFightTick = 0.5f;
		SetUpdateTick(updateNormalTick);
	}

	void NonPlayer::ResetFamiliarStatusByPlayerStat(int Str, int Dex, int Int, int Vit, int rebirthCount, bool isFirst)
	{
		//Lunia_IMPORTANT((L"NonPlayer::ResetFamiliarStatusByPlayerStat PlayerStat Str:{}, Dex:{}, Int:{}, Vit:{}, StageRebirthCount:{}", 
		//				  Str, Dex, Int, Vit, rebirthCount));

		float dollStrStatApplyRate, dollDexStatApplyRate, dollVitStatApplyRate, dollIntStatApplyRate;
		Database::DatabaseInstance().InfoCollections.Statuses.GetDollStatApplyRate(rebirthCount, dollStrStatApplyRate, dollDexStatApplyRate, dollVitStatApplyRate, dollIntStatApplyRate);
		
		Str *= dollStrStatApplyRate;
		Dex *= dollDexStatApplyRate;
		Int *= dollIntStatApplyRate;
		Vit *= dollVitStatApplyRate;

		//Lunia_IMPORTANT((L"NonPlayer::ResetFamiliarStatusByPlayerStat NonPlayerStat Str:{}, Dex:{}, Int:{}, Vit:{}, StageRebirthCount:{}, dollStatApplyRate:%f, dollDexStatApplyRate:%f, dollVitStatApplyRate:%f, dollIntStatApplyRate:%f",
		//			Str, Dex, Int, Vit, rebirthCount, dollStrStatApplyRate, dollDexStatApplyRate, dollVitStatApplyRate, dollIntStatApplyRate ));

		//Lunia_IMPORTANT((L"NonPlayer::ResetFamiliarStatusByPlayerStat Before_Str:{}, Before_Dex:{}", 
		//				  status.Str, status.Dex));

		//status.dmg.AddStr(-status.Str);
		//status.dmg.AddDex(-status.Dex);

		status.Str = Str;
		status.Dex = Dex;
		status.Int = Int;
		status.Vit = Vit;

		status.dmg.str = status.GetStr();
		status.dmg.dex = status.GetDex();

		RecalculateStatus(true);

		if (isFirst)
		{
			status.Mp = status.MaxMp;
			status.Hp = status.MaxHp;
		}

		//Lunia_IMPORTANT((L"NonPlayer::ResetFamiliarStatusByPlayerStat MaxHP:%f, HP:%f, hpPlus:%f, hpMul:%f", 
		//				  status.MaxHp, status.Hp, status.hpPlus, status.hpMul));

		//Lunia_IMPORTANT((L"NonPlayer::ResetFamiliarStatusByPlayerStat AtkMin:%f, AtkMax:%f, atkMinPlus:%f, atkMaxPlus:%f, atkMultiply:%f", 
		//				  status.dmg.AtkMin, status.dmg.AtkMax, status.dmg.atkMinPlus, status.dmg.atkMaxPlus, status.dmg.atkMultiply));

	}

	void NonPlayer::SetAIInfo(
		const Database::Info::StageInfo::Npc::IdleStateType& idleType,
		Database::Info::PathGroup* path,
		const float4& area,
		const float3& pos,
		const Database::Info::StageInfo::Npc::TransferType& transType,
		Serial owner)
	{
		NPCStatus.idleType = idleType;
		NPCStatus.paths = path;
		NPCStatus.area = area;
		NPCStatus.transType = transType;
		NPCStatus.familiar = owner;
		if (!transType)	// TransferType::HATELIST = 0
			triggerFlag |= Trigger::TriggerType::GOT_HATE;

		objectData.Position = pos;	//���⼭ ���� �ƹ� �������̳� �����ϰ� initialize���� ���� ������ ��ġ�� �˻��Ѵ�.
	}

	void NonPlayer::ModifyNpcInfoByPlayerCnt(float hpModifier, float xpModifier, float atkMultiplyModifier, float hitIgnoreMultiplyModifier)
	{
		//HP ����
		status.MaxHp *= hpModifier;
		status.Hp = status.MaxHp;
		//����ġ ����
		//NPCStatus.xp = (uint32)( (float)npcData.BaseCharacter.Xp * xpModifier );
		NPCStatus.xp = (uint32)( (float)NPCStatus.xp * xpModifier );

		//AddDamage(atkMultiplyModifier);
		atkMultiply = atkMultiplyModifier;
		hitIgnoreMultiply = hitIgnoreMultiplyModifier;
	}

	void NonPlayer::ModifyNpcInfoByRaidStageDiffy(unsigned int lv, const std::wstring& basicStat, uint32 xp)
	{
		NPCStatus.Lv = lv;
		NPCStatus.BasicStat = basicStat;
		NPCStatus.xp = xp;
	}

	void NonPlayer::ModifyNpcInfo( float hpFactor, float xpFactor, float intFactor, float dmgFactor, float updateNormalTick, float updateFightTick )
	{
		status.MaxHp *= hpFactor;
		status.Hp = status.MaxHp;

		status.Int = static_cast< int >( status.Int * intFactor );

		status.dmg.atkMultiply += dmgFactor;
		status.dmg.Recalculate( true );

		this->updateNormalTick = updateNormalTick;
		this->updateFightTick = updateFightTick;
		
		SetUpdateTick(updateNormalTick);
	}

	void NonPlayer::Initialize(Lunia::Math::RandomInt& rInt)
	{

		// NonPlayer�� ���� �ʱ� Ÿ���� �����Ѵ�
		// Type�� �Ϲ���, ������, ������3������ �ϳ� ( ���� �۾� )
		playertype = static_cast< NonPlayerType >( rInt.Next( 0, NonPlayerTypeCount ) );
		trycount   = 0;
		thinktime  = 0;
		////////////////////////////////////////////////////////////////////////////////////////////////

		posCnt = 0;
		bDead = false;
		//atkMultiply = 0.f; //fix by hak. atkMultiply�� ���� �׻� NPC�� �����ɶ� PlayerCnt���� ���� ������ �ǹǷ� ���⼭ �ʱ�ȭ�� �ʿ�� ����.

		NPCStatus.pathCnt = NPCStatus.paths ? rInt.Next( 0, (unsigned long)(NPCStatus.paths->size()) ) : 0;

		switch (NPCStatus.transType) {
		case Database::Info::StageInfo::Npc::TransferType::HATELIST :
			stateApproachTarget = &stateApproachTarget_HateList;
			stateCaution.Set(L"caution", this, &NonPlayer::stateCaution_HateListBegin, &NonPlayer::stateCaution_HateListUpdate, &NonPlayer::stateCaution_HateListEnd);
			break;
		default :
			stateApproachTarget = &stateApproachTarget_Sight;
			stateCaution.Set(L"caution", this, &NonPlayer::stateCaution_SightBegin, &NonPlayer::stateCaution_SightUpdate, &NonPlayer::stateCaution_SightEnd);
		}

		//Idle���´� object�� ���� �޶����� �ִ�.
		switch (NPCStatus.idleType) {
		case Info::StageInfo::Npc::IdleStateType::PATHFINDING :
			stateIdle = &stateIdlePathFinding;
			stateFightPtr = &stateFight;
			ChangeState(*stateIdle, updateNormalTick);
			break;
		case Info::StageInfo::Npc::IdleStateType::AREADEFENCE :
			stateIdle = &stateIdleDefence;
			stateFightPtr = &stateFight;
			ChangeState(*stateIdle, updateNormalTick);
			break;
		case Info::StageInfo::Npc::IdleStateType::SOLDIEROFFORTUNE :
		case Info::StageInfo::Npc::IdleStateType::DOLL :
			stateIdle = &stateIdleSoldierOfFortune;
			stateFightPtr = &stateFamiliarFight;
			//NPCStatus.familiarPtr = stageData->RetrievePlayer(NPCStatus.familiar);
			//if ( NPCStatus.familiarPtr ) {
			//	NPCStatus.familiarPtr->Tamed(Constants::Familiar::Type::Summon, this);
			//}
			//stateFightPtr = &stateFamiliarFight;
			ChangeState(stateFamiliarSummoned, updateNormalTick);
			break;
		case Info::StageInfo::Npc::IdleStateType::PET :
			stateIdle = &stateIdleSoldierOfFortune;			
			ChangeState(stateFamiliarSummoned, updateNormalTick);
			break;

		default :
			stateIdle = &stateIdleNone;
			stateFightPtr = &stateFight;
			initialPos= GetPosition();
			ChangeState(*stateIdle, updateNormalTick);
		}


		//FSM::UpdateTimeStamp �� �������� �������ش�.
		//���� �ð��� ���ÿ� ������ npc���� update �ð��� ���Ƽ� AI ������Ʈ�� �ѹ��� �̷������ �ʰ� �ϱ� ����
		//AI�� ������Ʈ ������ FSM::updateTick �ȿ��� �����ϰ� �ð��� ��������.
		updateTimeStamp = stageData->GetRandomFloat() * FSM::updateTick;

		agroClearTime = 0;
		agroCheckTime = 0;
		idleCheckTime = 0;

		
		timeToNextReselectTarget=ReselectTargetInterval;
		attackedByPlayer = false;
	}

	void NonPlayer::Die(Object* who)
	{
		//die�� �θ��� ���Ͱ� �׾�� �ϱ� ������, hp�� 0���� �ٲ��ش�.
		status.Hp = 0.0f;
		//���� ó���ÿ� xp�� �ְų� �ϴ°��� ���ϰ� üũ�ϱ� ���� bDead�÷��׸� true�� �ٲ��ش�.
		bDead = true;//����������, ��ƽ�� ������ ���������� ������ ����ġ�� �ߺ����� ���� �Ѹ��� ���� �����ϱ� ����.
		//Air Combo Clear
		((Actor*)this)->ClearAirAttackData();
	
		if (who)
			DistributeXp(who);
	}

	void NonPlayer::DistributeXp(Object* who)
	{
		std::vector<Player*> l;
		stageData->GetAreaPlayerByTeam(who->GetTeam(), objectData.Position, EXPRANGE, l);
		if (l.empty())
			return;
		std::vector<Player*>::iterator i=l.begin();
		uint16 totalLv=0;
		for ( ; i != l.end() ; ) {
			if ( !(*i)->CheckState(Info::StateInfo::Type::GHOST) ) {
				totalLv = totalLv + (*i)->GetLevel();
				++i;
			} else {
				i = l.erase( i );
			}
		}
		uint64 xp;
		
		float totalXp = (float)NPCStatus.xp * (float)(l.size());
		for ( i = l.begin() ; i != l.end() ; ++i ) {
			xp =  (uint32)( totalXp * (float)(*i)->GetLevel()/(float)totalLv );
			if ( xp > NPCStatus.xp ) {
				xp = NPCStatus.xp;
			}else if( xp < (NPCStatus.xp/2) ) {
				//���� xp���Ϸ� �ް� �Ǹ� ���� xp�� �����Ѵ�(50% ���Ϸδ� ���� �ʰ� ó��).
				xp = NPCStatus.xp/2;
			}

			//�����̿� ���� ó���� ��� �ؾ� ���� �𸣰ڴ�.
			//int32 levelGap = (int32)NPCStatus.npcInfo->Lv - (int32)(*i)->GetLevel();
			int32 levelGap = (int32)NPCStatus.Lv - (int32)(*i)->GetLevel();

			xp = (uint32)XRated::ExpFunctions::Stage::CalculateXpWithLevelGap((float)xp, levelGap);

			(*i)->XpGained( XRated::Constants::ExpAcquiredType::ExpNonPlayer, xp, true, true, GetSerial() );
		}
	}

	void NonPlayer::SetHP(float hp, float max)
	{
		//dead���� �϶��� hp�� ���̻� �ٲٰų� �Ҽ� ����. 2008-01-17 by kwind
		if( bDead == true ) {
			return;
		}
		Actor::SetHP(hp, max);
		NPCStatus.hpRate = characterData.Hp / characterData.MaxHp;
	}
	void NonPlayer::ChangeHpFromState(float hp, float max,Lunia::XRated::Serial donor)
	{
		//dead���� �϶��� hp�� ���̻� �ٲٰų� �Ҽ� ����. 2008-01-17 by kwind
		if( bDead == true ) {
			return;
		}
		Actor::ChangeHpFromState(hp, max,donor);
		NPCStatus.hpRate = characterData.Hp / characterData.MaxHp;
		if (status.Hp <= 0 && !bDead) {	//�׾���. ����ġ�� ���� �й�����.
			bDead = true;//����������, ��ƽ�� ������ ���������� ������ ����ġ�� �ߺ����� ���� �Ѹ��� ���� �����ϱ� ����.
			Die(stageData->Retrieve(donor));
			//Object* who = stageData->Retrieve(donor);
			//if ( who )
			//	Die(who);
		}
	}

	bool NonPlayer::DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
	{
		//dead���� �϶��� hp�� ���̻� �ٲٰų� �Ҽ� ����. 2008-01-17 by kwind
		if( bDead == true ) {
			return false;
		}
		float hp = status.Hp;
		//assert( dmgType >= 0 && dmgType <= Constants::DamageType::CNT );
		if ( dmgType < 0 || dmgType >= Constants::DamageType::CNT )
			dmgType = Constants::DamageType::PHYSICAL;
		float raceResist = (*NPCStatus.raceResist)[dmgType];

		//������ Ȯ��
		if ( who )
		{
			Constants::ObjectType type = who->GetType();
			switch( type ) 
			{
				case Constants::ObjectType::Projectile :
					{
						Player* creator = stageData->RetrievePlayer(static_cast<Projectile*>(who)->GetCreatorSerial());
						if(creator != NULL)
						{
							if ( GetHitIgnoreProb( creator->GetHitIgnoreValue() ) < stageData->GetRandomFloat() )
								sFlag |= Constants::StatusFlag::HitIgnore;
						}
					}
					break;
				case Constants::ObjectType::Player:
					{
						if ( GetHitIgnoreProb( ( (Actor*)who)->GetHitIgnoreValue() ) < stageData->GetRandomFloat() )
							sFlag |= Constants::StatusFlag::HitIgnore;
					}
					break;
			}
		}

		bool returnValue = Actor::DoDamage(who, creatorActionHash, minDmg * raceResist, maxDmg * raceResist, damage * raceResist, dmgType, stateID, attackType, sFlag);
		if (returnValue) 
		{
			if ( who && who->GetTeam() != GetTeam() ) 
			{
				switch ( who->GetType() ) 
				{
				case Constants::ObjectType::Projectile :
					{
						ChangeHate( ((Projectile*)who)->GetCreatorSerial(), hp-status.Hp );

						Player* creator = stageData->RetrievePlayer(static_cast<Projectile*>(who)->GetCreatorSerial());
						if(creator != NULL)
						{
							attackedByPlayer = true;
						}
					}
					break;
				case Constants::ObjectType::Player:
					{
						ChangeHate(who->GetSerial(), hp-status.Hp);
						attackedByPlayer = true;
					}
					break;
				default :
					ChangeHate(who->GetSerial(), hp-status.Hp);
				}
			}
			if (status.Hp <= 0 && !bDead) {	//�׾���. ����ġ�� ���� �й�����.
				bDead = true;//����������, ��ƽ�� ������ ���������� ������ ����ġ�� �ߺ����� ���� �Ѹ��� ���� �����ϱ� ����.
				Die(who);				
				//if ( who ) {
				//	DistributeXp(who);				
				//}
			}
		}

		return returnValue;
	}

	void NonPlayer::SetPosition(const float3& pos)
	{
		if ( IsState(stateIdlePathFinding) || IsState(stateIdleDefence) || IsState(stateIdleNone) || IsState(stateIdleSoldierOfFortune)) {// Idle �����ΰ�
			//actionTimeStamp = 0;
			if (actions.GetAction(L"")->GetActionClassType() == Info::ActionInfo::ActionClassType::MOVE
						&& Lunia::Math::Length<float>(targetPosition - pos) < objectData.Radius && NPCStatus.idleType != Info::StageInfo::Npc::IdleStateType::PET)
				//Command(Action::Action::CommandType::STOP, L"");
				stageData->AddTrigger(2, AI::Trigger::TriggerType::DESTINATION_ARRIVED, objectData.GameObjectSerial, objectData.Type,
										objectData.Position, objectData.Radius, 1, true);
		}

		Object::SetPosition(pos);
	}

	//float NonPlayer::GetResist(int type)
	//{
	//	float resist = Actor::GetResist(type);
	//	return resist;
	//}


	////////////////////////////////////////////////////////////////////////////////////////
	// Hate Functions	////////////////////////////////////////////////////////////////////
	void NonPlayer::RemoveHateTarget(Lunia::XRated::Serial serial)
	{
		if (NPCStatus.hateList.find(serial) == NPCStatus.hateList.end())
			return;
		NPCStatus.hateList.erase(serial);

		if ( NPCStatus.hateList.empty() )
			NPCStatus.target = 0;
		else
			RecalculateMostHateTarget();
	}

	void NonPlayer::ChangeHate(Lunia::XRated::Serial serial, float hate)
	{
		if (NPCStatus.hateList.find(serial) == NPCStatus.hateList.end()) {	//ù �մ��̱�
			NPCStatus.hateList[serial] = hate;
		} else {
			NPCStatus.hateList[serial] += hate;
		}
		if (NPCStatus.hateList[serial] <= 0)
			NPCStatus.hateList.erase(serial);

		if ( NPCStatus.hateList.empty() )
			NPCStatus.target = 0;
		else
			RecalculateMostHateTarget();

#ifdef _SINGLE
		if ( stageData->showNPCInfo == 2) {
			std::wstring msg;
			for( std::map<Serial, float>::iterator i = NPCStatus.hateList.begin() ; i != NPCStatus.hateList.end() ; ++i) {
				Object* obj = stageData->Retrieve((*i).first);
				if ( !obj ) continue;
				msg += StringUtil::Format(L"%s %f\n", obj->GetName().c_str(), (*i).second);
			}
			stageData->NPCDebugMessage(GetSerial(), msg);
		}
#endif
	}

	void NonPlayer::ReduceHate(float hate)
	{
		std::map<Serial, float>::iterator i = NPCStatus.hateList.begin(), end = NPCStatus.hateList.end();
		for (;i != end;) {
			(*i).second -= hate;

			if ((*i).second <= 0)
				i = NPCStatus.hateList.erase(i);
			else ++i;
		}

		if ( NPCStatus.hateList.empty() )
			NPCStatus.target = 0;
		else
			RecalculateMostHateTarget();

#ifdef _SINGLE
		if ( stageData->showNPCInfo == 2) {
			std::wstring msg;
			for( std::map<Serial, float>::iterator i = NPCStatus.hateList.begin() ; i != NPCStatus.hateList.end() ; ++i) {
				Object* obj = stageData->Retrieve((*i).first);
				if ( !obj ) continue;
				msg += StringUtil::Format(L"%s %f\n", obj->GetName().c_str(), (*i).second);
			}
			stageData->NPCDebugMessage(GetSerial(), msg);
		}
#endif
	}

	Lunia::XRated::Serial NonPlayer::RecalculateMostHateTarget()
	{
		if (NPCStatus.tauntDuration > 0 && NPCStatus.target)//Ÿ��Ʈ���� ���¸� ������� �ʰ� �׳� target�� ����.
			return NPCStatus.target;
		
		if( NPCStatus.npcInfo->IgnoreHate == true )
		{
			if ( timeToNextReselectTarget < 0)
			{
				return SetTargetToNearest();
			}
			else
			{
				Actor* enemy = stageData->RetrieveActor( NPCStatus.target );
				if( enemy != NULL)
				{
						if ( IsAbleToRecognize( enemy) == false && enemy->GetType() == XRated::Constants::ObjectType::Player) // �׾��ų� ���� ���ų� ���
							return SetTargetToNearest();
						else
						{
							return NPCStatus.target;
						}
				}
				else
				{
					//stageData�� ���� ����. �׳� ������ �ִ� �ָ� ã��.
					return SetTargetToNearest();
				}
			}
		}

		SetTargetToMostHate(); // Hate�� ���� ���� ���� ã�� ������� �ٲ۴�.
		return NPCStatus.target;
	}

	void NonPlayer::Taunted (Lunia::XRated::Serial serial, float duration)
	{
		NPCStatus.tauntDuration = duration;
		NPCStatus.target = serial;
	}



	bool NonPlayer::Update(float dt, IGameStateDatabase* db)
	{
		thinktime += dt;

		agroClearTime += dt;
		agroCheckTime += dt;

		timeToNextReselectTarget -= dt;
		if( timeToNextReselectTarget < 0 && NPCStatus.npcInfo->IgnoreHate == true)
		{
			RecalculateMostHateTarget();
		}

		FSM::Update(dt);
		Actor::Update(dt, db);

		if (status.Hp <= 0) {//�׾���.
			ChangeState(stateEndOperation, updateNormalTick);
			if ( NPCStatus.familiarPtr ) //���ο��� �ڽ��� ������ �˸���.
				NPCStatus.familiarPtr->FamiliarDestroyed(this);
			return true;
		}

		if (NPCStatus.tauntDuration > 0) {
			NPCStatus.tauntDuration -= dt;
			if (NPCStatus.tauntDuration <= 0) {
				NPCStatus.tauntDuration = 0;
				RecalculateMostHateTarget();
			}
		}
		return false;
	}

	void NonPlayer::MasterCommandMeAttack(const float3& pos)
	{
		targetPosition = pos;
		ChangeState(stateFamiliarAttackGround, updateFightTick);
	}

	void NonPlayer::MasterCommandMeGuard()
	{
		if ( !NPCStatus.familiarPtr ) {
			Logger::GetInstance().Exception(L"[NonPlayer::MasterCommandMeGuard] I don't have any familiar. [{0}/{1}]", GetSerial(), GetName().c_str());
			return;
		}
		initialPos = NPCStatus.familiarPtr->GetFamiliarPosition(this);
		Object::SetPosition(initialPos); //NonPlayer::SetPosition()�� ���� ȣ���� ������ ����.
		SetAction(Database::Info::HashTable::Actions::Stand, NULL, true);
		stateIdle = &stateIdleNone;
		ChangeState(*stateIdle, updateNormalTick);
	}

	void NonPlayer::MasterCommandMeCome(const float3& pos)
	{
		SetPosition(pos);
		//SetAction(Database::Info::HashTable::Actions::Stand, NULL, true);
		ClearHateList();
		stateIdle = &stateIdleSoldierOfFortune;
		ChangeState(*stateIdle, updateNormalTick);
	}

	void NonPlayer::MasterCommandMeSuicide(bool bSacrifice)
	{
		if ( bSacrifice ) {
			if(NPCStatus.idleType == Info::StageInfo::Npc::IdleStateType::PET && !SetAction(14193212/*End_Action*/, NULL, true))
				SetHP(0);
			else if ( !SetAction(Database::Info::HashTable::Actions::Sacrifice, NULL, true) ) //��� �׼��� ���� �� ������.#endif
				SetHP(0);
			else
				ChangeState(stateEndOperation, updateNormalTick);
		} else {
			SetHP(0);
		}
	}

	void NonPlayer::MasterCommand(uint32 actionHash, bool bForce)
	{
		SetAction(actionHash, NULL, bForce);
	}

	void NonPlayer::MasterPraiseMe(float /*hpAmount*/)
	{
		SetAction(Database::Info::HashTable::Actions::Praise, NULL, true);
	}
	void NonPlayer::SetPartyCntWhenCreated(int cnt)
	{
		npcData.partyCntWhenCreated = cnt;
	}

	float NonPlayer::GetIncreasedDamageRate(int str)
	{
		if (NPCStatus.idleType == Info::StageInfo::Npc::IdleStateType::SOLDIEROFFORTUNE ||
			NPCStatus.idleType == Info::StageInfo::Npc::IdleStateType::DOLL) 
		{
			return StatusFunctions::GetIncreasedDamageRate(str);
		}

		//return StatusFunctions::GetIncreasedDamageRateForNonPlayer(str);
		return 1;
	}

	bool NonPlayer::IsDropEventNPC()
	{
		if (npcData.Npc == Lunia::XRated::NonPlayerData::NpcType::EpisodeBoss ||
			npcData.Npc == Lunia::XRated::NonPlayerData::NpcType::SemiBoss ||
			npcData.Npc == Lunia::XRated::NonPlayerData::NpcType::StageBoss )
			return false;

		if (!attackedByPlayer) return false;

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation
	bool NonPlayer::HandleTrigger(AI::Trigger& trigger)
	{
		switch (trigger.type) {
		case Trigger::TriggerType::DESTINATION_ARRIVED :
			Command(Action::Action::CommandType::STOP, 0);
			trigger.bExpired = true;
			return true;
		}
		return false;
	}

	bool NonPlayer::IsAbleToRecognize(Actor* actor)
	{
		const NPCMap& npcmap = stageData->GetNPCs();
		NPCMap::const_iterator it = stageData->GetNPCs().find( actor->GetSerial() );

		if ( actor->CheckState(Info::StateInfo::Type::GHOST) )
			return false;
		else if ( actor->CheckState(Info::StateInfo::Type::INVISIBLE) && !CheckState(Info::StateInfo::Type::DETECTINVISIBLE) )
			return false;
		else if ( it != npcmap.end() && it->second->NPCStatus.idleType == Info::StageInfo::Npc::IdleStateType::PET )
			return false;
		return true;
	}

	bool NonPlayer::CheckSurroundEnemy()
	{
		// �Ǹ� ���¸� ������ �� ����
		if ( CheckState(Database::Info::StateInfo::Type::BLIND) )
			return false;

		std::vector<Actor*> l;
		// retrieve enemy list from stagedatabase
		stageData->GetAreaObjectForSector9(objectData.Position, NPCStatus.sight, Constants::AllianceType::ExTeam, characterData.Team, this, l);
		
		if ( l.empty() ) // no enemy in sight. return
			return false;

		// Enemy detected
		// HateList�� ù��° �ִ� �𿡰� �޷� ����.(���� ����)

		Actor *actor;
		for ( std::vector<Actor*>::iterator i = l.begin() ; i != l.end() ; ++i )
		{
			actor = (*i);
			if ( !IsAbleToRecognize(actor) )
				continue;

			ChangeHate(actor->GetSerial(), status.MaxHp * 0.01f);
			
			ChangeState( *stateApproachTarget, updateNormalTick );
			
			updateTimeStamp = FSM::updateTick;

			return true;
		}

		return false;
	}

	void NonPlayer::SetAIPath(Database::Info::PathGroup* path)
	{
		NPCStatus.paths = path;
		NPCStatus.pathCnt = path ? stageData->GetRandomInt( 0, (unsigned long)(path->size()) ) : 0;
		posCnt = 0;
	}

	void NonPlayer::SetAIArea(const float4& area)
	{
		NPCStatus.area = area;
	}

	void NonPlayer::SetIdleState(Info::StageInfo::Npc::IdleStateType type)
	{
		NPCStatus.idleType = type;
		switch (type) {
		case Info::StageInfo::Npc::IdleStateType::STAND :
			stateIdle = &stateIdleNone;
			initialPos = GetPosition();
			break;
		case Info::StageInfo::Npc::IdleStateType::AREADEFENCE :
			stateIdle = &stateIdleDefence;
			break;
		case Info::StageInfo::Npc::IdleStateType::PATHFINDING :
			stateIdle = &stateIdlePathFinding;
			break;
		case Info::StageInfo::Npc::IdleStateType::DOLL:
		case Info::StageInfo::Npc::IdleStateType::SOLDIEROFFORTUNE :
		case Info::StageInfo::Npc::IdleStateType::PET :
			stateIdle = &stateIdleSoldierOfFortune;
			break;
		}
		ChangeState(*stateIdle, updateNormalTick);
	}

	void NonPlayer::IdleNoneBegin()
	{
		float distance = Lunia::Math::Length<float>(initialPos - objectData.Position);
		if ( distance > objectData.Radius ) {//ó�� ��ġ���� ��� �ִ�.
			targetPosition = initialPos;
			AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
			currentDirection = collisionCnt = 0;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else {
			Command(Action::Action::CommandType::STOP, 0);
		}

		idleCheckTime = 0;
	}

	void NonPlayer::IdleNoneUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"IdleNoneUpdate()");
#endif

		if ( NPCStatus.target ) { //�������� �����ִµ� ����Ʈ�� �־���.
			ChangeState( *stateApproachTarget, updateNormalTick );
		}

		if ( CheckSurroundEnemy() )
			return;

		float distance = Lunia::Math::Length<float>(initialPos - objectData.Position);

		if ( distance > objectData.Radius )
		{
			idleCheckTime += AI::FSM::updateTick;
			if ( idleCheckTime > 60.0f ) {
				float3 availablePos = stageData->GetAvailablePos(initialPos, NPCStatus.npcInfo->Radius, this);
				idleCheckTime = 0;
				if ( availablePos != float3(0,0,0) ) {
					Object::SetPosition(availablePos);
					objectData.Position = availablePos;
					Command(Action::Action::CommandType::STOP, 0);
					return;
				}
			}

			//ó�� ��ġ���� ��� �ִ�.
			uint32 dir = targetDirection[currentDirection];
			AI::DecideDirection(objectData.Position, initialPos, targetDirection);
			if (bCollide) {//�浹 �����ΰ�.
				++collisionCnt;
				//��� �̰� 6������ �� AI������Ʈ ƽ���� ����Ǵ°� ���� �����̴�.
				//4�踦 �ؼ� 4ƽ���� �ѹ��� ������ �ٲ�� �ϱ� ���� ������ ��.
				if (collisionCnt == 24)
					collisionCnt = 0;
				currentDirection = collisionCnt/4;
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
			} else {
				currentDirection = collisionCnt = 0;
				if (dir != targetDirection[currentDirection] ||
					(!CheckState(Database::Info::StateInfo::Type::MOVE)  && !CheckState(Database::Info::StateInfo::Type::STEP)) )
				{
					Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
				}
			}
		} else if ( CheckState(Database::Info::StateInfo::Type::MOVE)  || CheckState(Database::Info::StateInfo::Type::STEP) ) {
			Command(Action::Action::CommandType::STOP, 0);
		}
	}

	void NonPlayer::IdleNoneEnd()
	{
		targetPosition = objectData.Position;
	}

	void NonPlayer::IdlePathFindingBegin()
	{
		ClearHateList();

		//path ���� ���� ����� ���ް����� ����Ʈ�� �˻��ϰ� Ÿ������ ��´�.
		std::vector<float2>& path = NPCStatus.paths->at(NPCStatus.pathCnt);
		std::vector<float2>::iterator i = path.begin(), end = path.end();
		float2 sourcePos( objectData.Position.x, objectData.Position.z );
		float2 targetPos, currentPos;
		float2 direction;

		if (this->posCnt) {//�ڽ��� ���� ��ǥ ������ ���ް����Ѱ��� ���� üũ
			targetPos = path[this->posCnt];
			currentPos = sourcePos;
			float len = sqrt( pow(targetPos.x-sourcePos.x, 2) + pow(targetPos.y-sourcePos.y, 2) );
			direction.SetVector( (targetPos.x-sourcePos.x)/len, (targetPos.y-sourcePos.y)/len );
			uint32 cnt = (uint32)len+2;	//�ѹ��� �����Ǵ� �� ������, �ѹ��� �������۽� ���鼭 �����ϱ� ������, �� +2�� ���ش�.
			while(--cnt) {
				currentPos += direction;
				if (!stageData->IsAbletoLocateInMap(float3(currentPos.x, 0, currentPos.y), objectData.Radius))
					break;
			}
			if (!cnt) {	//���� ����. �ش� ����Ʈ�� ��ǥ�� �� �����.
				targetPosition.SetVector(targetPos.x, 0, targetPos.y);
				this->posCnt = posCnt;
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				currentDirection = collisionCnt = 0;
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
				return;
			}
		}

		//���� ����Ʈ�� ���� �Ұ�. ��� ����Ʈ�� ó������ �˻�
		for (uint32 posCnt=0; i != end ; ++i, ++posCnt) {
            targetPos = (*i);
			currentPos = sourcePos;
			float len = sqrt( pow(targetPos.x-sourcePos.x, 2) + pow(targetPos.y-sourcePos.y, 2) );
			direction.SetVector( (targetPos.x-sourcePos.x)/len, (targetPos.y-sourcePos.y)/len );
			uint32 cnt = (uint32)len+2;	//�ѹ��� �����Ǵ� �� ������, �ѹ��� �������۽� ���鼭 �����ϱ� ������, �� +2�� ���ش�.
			while(--cnt) {
				currentPos += direction;
				if (!stageData->IsAbletoLocateInMap(float3(currentPos.x, 0, currentPos.y), objectData.Radius))
					break;
			}
			if (!cnt) {	//���� ����. �ش� ����Ʈ�� ��ǥ�� �� �����.
				targetPosition.SetVector(targetPos.x, 0, targetPos.y);
				this->posCnt = posCnt;
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				currentDirection = collisionCnt = 0;
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
				return;
			}
		}
		idleCheckTime = 0;
	}

	void NonPlayer::IdlePathFindingUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"IdlePathFindingUpdate()");
#endif

		if ( NPCStatus.target ) { //�������� �����ִµ� ����Ʈ�� �־���.
			ChangeState( *stateApproachTarget, updateNormalTick );
		}

		if ( CheckSurroundEnemy() )
			return;

		//��ǥ ������ �����ߴ°�.
		if (Lunia::Math::Length<float>(targetPosition - objectData.Position) < objectData.Radius) {
			idleCheckTime = 0;
			//���� ��ǥ�� ����.
			if ( ++posCnt < ( NPCStatus.paths->at(NPCStatus.pathCnt) ).size() ) {
				targetPosition.SetVector( (*NPCStatus.paths)[NPCStatus.pathCnt][posCnt].x, 0, (*NPCStatus.paths)[NPCStatus.pathCnt][posCnt].y );
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				currentDirection = collisionCnt = 0;
				actionTimeout = 1.5f;
				actionTimeStamp = 0;
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
				stageData->NPCArrived(this, NPCStatus.pathCnt, posCnt);
			} else {
				//���ڸ��� ���缱��.
				initialPos = GetPosition(); //�ڽ��� �ʱ� ��ġ�� ���� ��ġ�� �����ؼ� ���� ��ġ�� ���ư����� �ൿ�� ���´�.
				stateIdle = &stateIdleNone;
				ChangeState(*stateIdle, updateNormalTick);
				stageData->NPCArrived(this, NPCStatus.pathCnt, -1/*������ ����Ʈ�� �����ߴ�.*/ );
				return;
			}
		} else { //��ǥ������ ���� �������� ���ߴ�.
			idleCheckTime += AI::FSM::updateTick;
			if ( idleCheckTime > 60.0f ) { //1���� �ѵ��� ���� ��ǥ������ ���� ���ߴ�.
				float3 availablePos = stageData->GetAvailablePos(targetPosition, NPCStatus.npcInfo->Radius, this);
				idleCheckTime = 0;
				if ( availablePos != float3(0,0,0) ) {
					objectData.Position = availablePos;
					Command(Action::Action::CommandType::STOP, 0);
					return;
				}
			}
		}
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);

		if (bCollide) {//�浹 �����ΰ�.
			++collisionCnt;
			//��� �̰� 6������ �� AI������Ʈ ƽ���� ����Ǵ°� ���� �����̴�.
			//4�踦 �ؼ� 4ƽ���� �ѹ��� ������ �ٲ�� �ϱ� ���� ������ ��.
			if (collisionCnt == 24)
				collisionCnt = 0;
			currentDirection = collisionCnt/4;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else {
			currentDirection = collisionCnt = 0;
			if (dir != targetDirection[currentDirection] ||
				(!CheckState(Database::Info::StateInfo::Type::MOVE)  && !CheckState(Database::Info::StateInfo::Type::STEP)) )
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		}
	}

	void NonPlayer::IdlePathFindingEnd()
	{
		targetPosition = objectData.Position;
	}

	void NonPlayer::IdleDefenceBegin()
	{
		ClearHateList();

		targetPosition.SetVector(stageData->GetRandomFloat()*(NPCStatus.area.z-NPCStatus.area.x)+NPCStatus.area.x, 0,
									stageData->GetRandomFloat()*(NPCStatus.area.y-NPCStatus.area.w)+NPCStatus.area.w);
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
		currentDirection = collisionCnt = 0;
		actionTimeout = 0;	//time out �� 0�̶� ���� �����̴� ���¶�°�.
		Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		idleCheckTime = 0;
	}

	void NonPlayer::IdleDefenceUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"IdleDefenceUpdate()");
#endif

		if ( NPCStatus.target ) { //�������� �����ִµ� ����Ʈ�� �־���.
			ChangeState( *stateApproachTarget, updateNormalTick );
			return;
		}

		if ( CheckSurroundEnemy() )
			return;

		if (actionTimeout) {
			if (actionTimeStamp < actionTimeout)	//��� ���¶�� �׳� �н�
				return;

			targetPosition.SetVector(stageData->GetRandomFloat()*(NPCStatus.area.z-NPCStatus.area.x)+NPCStatus.area.x, 0,
										stageData->GetRandomFloat()*(NPCStatus.area.y-NPCStatus.area.w)+NPCStatus.area.w);
			AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
			actionTimeout = 0;	//time out �� 0�̶� ���� �����̴� ���¶�°�.
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		}

		//��ǥ ������ �����ߴ°�.
		if (Lunia::Math::Length<float>(targetPosition - objectData.Position) < objectData.Radius) {
			//�����ߴٸ� �� ���� ����.
			actionTimeout = stageData->GetRandomFloat()*4.0f;
			actionTimeStamp = 0;
		} else {
			idleCheckTime += AI::FSM::updateTick;
			if ( idleCheckTime > 60.0f ) { //1���� �ѵ��� ���� ��ǥ������ ���� ���ߴ�.
				float3 availablePos = stageData->GetAvailablePos(targetPosition, NPCStatus.npcInfo->Radius, this);
				if ( availablePos != float3(0,0,0) ) {
					objectData.Position = availablePos;
					Command(Action::Action::CommandType::STOP, 0);
					idleCheckTime = 0;
					return;
				}
				idleCheckTime = 0;
			}

			uint32 dir = targetDirection[currentDirection];
			AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
			if (dir != targetDirection[currentDirection] ||
				(!CheckState(Database::Info::StateInfo::Type::MOVE)  && !CheckState(Database::Info::StateInfo::Type::STEP)) )
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
			actionTimeStamp = 0;
		}
	}

	void NonPlayer::IdleDefenceEnd()
	{
		targetPosition = objectData.Position;
	}

	void NonPlayer::IdleSoldierOfFortuneBegin()
	{
		if ( !NPCStatus.familiarPtr )
			return;

		targetPosition = NPCStatus.familiarPtr->GetFamiliarPosition(this);

		//SetPosition(targetPosition);
		Object::SetPosition(targetPosition); //NonPlayer::SetPosition�� ���� ȣ�� �� ������ ����.
		SetDirection(NPCStatus.familiarPtr->GetDirection()); //��ȯ�ڿ� ���� ������ �ٶ󺻴�.

		if( NPCStatus.idleType == Info::StageInfo::Npc::IdleStateType::PET )
		{
			SetAction(Database::Info::HashTable::Actions::Stand, NULL, true);
		}
		else
		{
			SetAction(Database::Info::HashTable::Actions::Reformation, NULL, true);
		}

		currentDirection=0;
		AI::DecideDirection(objectData.Position, NPCStatus.familiarPtr->GetFamiliarPosition(this), targetDirection);
		//if ( !CheckState(Database::Info::StateInfo::Type::MOVE) && !CheckState(Database::Info::StateInfo::Type::STEP) )
		//	Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		actionTimeStamp = 0;
	}

	void NonPlayer::IdleSoldierOfFortuneUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"IdleSoldierOfFortuneUpdate()");
#endif
		if ( !NPCStatus.familiarPtr || NPCStatus.target ) { //�������� �����ִµ� ����Ʈ�� �־���.
			ChangeState( *stateApproachTarget, updateNormalTick );
			return;
		}

		float3 targetPosition = NPCStatus.familiarPtr->GetFamiliarPosition(this);
		float length = Math::Length<float>(targetPosition-objectData.Position);
		Action::Action::CommandType cmdType = Action::Action::CommandType::MOVE;

		switch( NPCStatus.idleType )
		{
		case Info::StageInfo::Npc::IdleStateType::DOLL:
		case Info::StageInfo::Npc::IdleStateType::SOLDIEROFFORTUNE:
			{
				if ( length < 8.0f ) {
					if (CheckState(Database::Info::StateInfo::Type::MOVE) || CheckState(Database::Info::StateInfo::Type::STEP)) {
						if(NPCStatus.familiarPtr)
							SetDirection(NPCStatus.familiarPtr->GetDirection());
						Command(Action::Action::CommandType::STOP, 0);
					}
					return;
				} else if ( length > 90.0f ) { //90���� �Ѱ� �־����ٸ� ���� ����.
					ChangeState(*stateIdle, updateNormalTick);
				} else if ( length > 40.0f ) { //�Ÿ��� ���� ��������.
					cmdType = Action::Action::CommandType::DASH;
				}
			}
			break;

		case Info::StageInfo::Npc::IdleStateType::PET:
			{
				if ( length < 10.0f ) {
					if (CheckState(Database::Info::StateInfo::Type::MOVE) || CheckState(Database::Info::StateInfo::Type::STEP)) {
						if(NPCStatus.familiarPtr)
							SetDirection(NPCStatus.familiarPtr->GetDirection());
						Command(Action::Action::CommandType::STOP, 0);
					}
					else
					{
						if(NPCStatus.familiarPtr)
						{
							if( objectData.Direction != NPCStatus.familiarPtr->GetDirection() )
							{
								SetDirection(NPCStatus.familiarPtr->GetDirection());
								Command(Action::Action::CommandType::STOP, 0);
							}
						}
					}
					return;
				} else if ( length > 90.0f ) { //90���� �Ѱ� �־����ٸ� ���� ����.
					ChangeState(*stateIdle, updateNormalTick);
				} 
				else if ( length > 40.0f ) { //�Ÿ��� ���� ��������.
					cmdType = Action::Action::CommandType::DASH;
				}
			}
			break;
		default:
			{
			Logger::GetInstance().Error( L"IdleSoldierOfFortuneUpdate : it's not soldierOfFortune");
			}
			break;
		}

		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
		if (dir == targetDirection[currentDirection]) {
			if ( !CheckState(Database::Info::StateInfo::Type::MOVE)  && !CheckState(Database::Info::StateInfo::Type::STEP) ) {
				Command(cmdType, targetDirection[currentDirection]);
			} else {
				switch ( cmdType ) {
				case Action::Action::CommandType::MOVE :
					if ( GetActionNameHash() == Database::Info::HashTable::Actions::Run )
					{
						Command(cmdType, targetDirection[currentDirection]);
					}
					break;
				case Action::Action::CommandType::DASH :
					if ( GetActionNameHash() == Database::Info::HashTable::Actions::Walk )
					{
						Command(cmdType, targetDirection[currentDirection]);
					}
					break;
				default :
					break;
				}
			}
		} else {
			Command(cmdType, targetDirection[currentDirection]);
		}
	}

	void NonPlayer::stateApproachTarget_SightBegin()
	{
		currentDirection = 0;
		collisionCnt = 0;
	}

	void NonPlayer::stateApproachTarget_SightUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"stateApproachTarget_SightUpdate()");
#endif

		if (NPCStatus.target == 0) {//��� �������� ����. ������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		
		//������ �� �ִ°�.
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//������ �����ų� �ν��� �� ���ų�, �þ߸� �����. hatelist ���� ���ش�.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight ) {
			RemoveHateTarget(NPCStatus.target);
			ChangeState(*stateIdle, updateNormalTick);
			return;
		} else if (distance <= NPCStatus.attackRange) {//�̳��Ű ���ɷȾ�. ������
			ChangeState(*stateFightPtr, updateFightTick);
			return;
		}

		//�þ߾ȿ� ������ �����Ҹ��� �Ÿ��� �ƴϴ�. �ٰ�����.
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		currentDirection = 0;

		if (bCollide) {//�浹 �����ΰ�.
			++collisionCnt;
			//��� �̰� 6������ �� AI������Ʈ ƽ���� ����Ǵ°� ���� �����̴�.
			//4�踦 �ؼ� 4ƽ���� �ѹ��� ������ �ٲ�� �ϱ� ���� ������ ��.
			if (collisionCnt == 24)
				collisionCnt = 0;
			currentDirection = collisionCnt/4;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else collisionCnt = 0;

		if (dir != targetDirection[currentDirection] ||
			(!CheckState(Database::Info::StateInfo::Type::MOVE)  && !CheckState(Database::Info::StateInfo::Type::STEP)) )
			Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);
	}

	void NonPlayer::stateApproachTarget_HateListBegin()
	{
		collisionCnt = 0;
		currentDirection = 0;
	}

	void NonPlayer::stateApproachTarget_HateListUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"stateApproachTarget_HateListUpdate()");
#endif

		//�ڽ��� ó�� ��ġ���� ���� ����°��� �˻��Ѵ�.
		//if ( Lunia::Math::Length<float>(objectData.Position - targetPosition) > NPCStatus.sight*2) //�ڽ��� ������ ��ġ���� �־����� hate�� �پ���.
		//	ReduceHate(characterData.MaxHp/30.0f);
		if ( agroClearTime > 30 ) {
			agroClearTime = 0;
			ClearHateList();
			ChangeState(*stateIdle, updateNormalTick);
		}
		if ( agroCheckTime > 15 ) {
			agroCheckTime = 0;
			std::vector<Actor*> l;
			// retrieve enemy list from stagedatabase
			stageData->GetAreaObjectForSector9(objectData.Position, NPCStatus.sight, Constants::AllianceType::ExTeam, characterData.Team, this, l);
			if ( l.empty() ) {// no enemy in sight. return
				ClearHateList();
				ChangeState(*stateIdle, updateNormalTick);
			}
		}

		if (NPCStatus.target == 0) {//��� �������� ����. ������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		//������ �� �ִ°�.
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj) ) {	//�󾾱�.. ������ ����. ���� ����� ��������.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		//�þ߾ȿ� ������ �����Ҹ��� �Ÿ��� �ƴϴ�. �ٰ�����.
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		currentDirection = 0;

		if ( Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) <= NPCStatus.attackRange ) {//�̳��Ű ���ɷȾ�. ������
			ChangeState(*stateFightPtr, updateFightTick);
			return;
		}

		if (bCollide) {//�浹 �����ΰ�.
			++collisionCnt;
			//��� �̰� 6������ �� AI������Ʈ ƽ���� ����Ǵ°� ���� �����̴�.
			//4�踦 �ؼ� 4ƽ���� �ѹ��� ������ �ٲ�� �ϱ� ���� ������ ��.
			if (collisionCnt == 24)
				collisionCnt = 0;
			currentDirection = collisionCnt/4;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else collisionCnt = 0;

		if (dir != targetDirection[currentDirection] ||
			(!CheckState(Database::Info::StateInfo::Type::MOVE) && !CheckState(Database::Info::StateInfo::Type::STEP)) )
		{
			std::wstring strtype;

			switch( playertype )
			{
			case NonPlayerType::Away:	strtype = L"������"; break;
			case NonPlayerType::Normal: strtype = L"�Ϲ���"; break;
			case NonPlayerType::Stop:	strtype = L"������"; break;
			}

			// �� �ൿ�� ��� ���� ���� �Ѵ�.
			if( Think() == 1 )
			{
				ChangeState( stateCaution, updateNormalTick );
			}else{
				Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);

			}
		}
	}

	void NonPlayer::stateCaution_HateListUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"stateCaution_HateListUpdate()");
#endif

		//�ڽ��� ó�� ��ġ���� ���� ����°��� �˻��Ѵ�.
		//if ( Lunia::Math::Length<float>(objectData.Position - targetPosition) > NPCStatus.sight*2)
		//	ReduceHate(characterData.MaxHp/30.0f);

		// ���� Target �̶� ������??
		if( NPCStatus.idleType == Info::StageInfo::Npc::IdleStateType::PET )
		{
			ChangeState(*stateIdle, updateNormalTick);
		}
		else
		{
			ChangeState(*stateApproachTarget, updateNormalTick);
		}
	}

	void NonPlayer::stateCaution_SightUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"stateCaution_SightUpdate()");
#endif
		// ���� Target �̶� ������??
		if( NPCStatus.idleType == Info::StageInfo::Npc::IdleStateType::PET )
		{
			ChangeState(*stateIdle, updateNormalTick);
		}
		else
		{
			ChangeState(*stateApproachTarget, updateNormalTick);
		}
	}

	void NonPlayer::WarningBegin()
	{
		//��. �ϴ��� ���缱��.
		//actions.SetAction(L"Stand", this, NULL, *stageData);
		Command(Action::Action::CommandType::STOP, 0);
	}

	void NonPlayer::WarningUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"WarningUpdate()");
#endif
		if ( NPCStatus.target ) { //�������� �����ִµ� ����Ʈ�� �־���.
			ChangeState( *stateApproachTarget, updateNormalTick );
		}

		if ( CheckSurroundEnemy() )
			return;

		ChangeState(*stateIdle, updateNormalTick);
	}

	void NonPlayer::WarningEnd()
	{
	}

	void NonPlayer::FightBegin()
	{
		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand)
			Command(Action::Action::CommandType::STOP, 0);
	}

	void NonPlayer::FightUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"FightUpdate()");
#endif

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj) ) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		float3 tempDirection = objectData.Direction;
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
		bufferedDirection = objectData.Direction;
		//bufferedDirection = AI::DirectionToFloat3(targetDirection[0]);

		if (!DoAttack(obj)) objectData.Direction = tempDirection;
		ChangeState(stateCaution, updateNormalTick);
	}

	XRated::Constants::Familiar::Type NpcIdleTypeToFamiliarType(XRated::Database::Info::StageInfo::Npc::IdleStateType type)
	{
		switch(type)
		{
		case XRated::Database::Info::StageInfo::Npc::DOLL:
			return Constants::Familiar::Doll;
		case XRated::Database::Info::StageInfo::Npc::PET:
			return Constants::Familiar::Type::Pet;
		}

		return XRated::Constants::Familiar::Type::Summon;
	}

	void NonPlayer::FamiliarSummonedBegin()
	{
		//familiar�� PlayerCnt�� ���� atkMultiplyModifier�� ���� ��ȭ���� ����Ѵ�.
		//AddDamage(-atkMultiply);
		atkMultiply = 0;

		NPCStatus.familiarPtr = stageData->RetrievePlayer(NPCStatus.familiar);

		if(NPCStatus.familiarPtr)
		{
			if(NPCStatus.familiarPtr->Tamed(NpcIdleTypeToFamiliarType(NPCStatus.idleType), this) == false &&
				NPCStatus.idleType == Database::Info::StageInfo::Npc::PET)
				MasterCommandMeSuicide(true);
		}
		else
		{
			if(NPCStatus.idleType == Database::Info::StageInfo::Npc::PET)
				MasterCommandMeSuicide(true);
		}
	}

	void NonPlayer::FamiliarSummonedUpdate()
	{
		if (Actor::GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
			ChangeState(stateCaution, updateNormalTick);
			//ChangeState(*stateIdle);
	}

	void NonPlayer::FamiliarAttackGroundBegin()
	{
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
		currentDirection = collisionCnt = 0;
		actionTimeout = 0;	//time out �� 0�̶� ���� �����̴� ���¶�°�.
		Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		idleCheckTime = 0;
	}

	void NonPlayer::FamiliarAttackGroundUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"FamiliarAttackGroundUpdate()");
#endif

		if ( NPCStatus.target ) { //�������� �����ִµ� ����Ʈ�� �־���.
			ChangeState( *stateApproachTarget, updateNormalTick );
			return;
		}

		if ( CheckSurroundEnemy() )
			return;

		if (actionTimeout) {
			if (actionTimeStamp < actionTimeout)	//��� ���¶�� �׳� �н�
				return;

			targetPosition.SetVector(stageData->GetRandomFloat()*(NPCStatus.area.z-NPCStatus.area.x)+NPCStatus.area.x, 0,
										stageData->GetRandomFloat()*(NPCStatus.area.y-NPCStatus.area.w)+NPCStatus.area.w);
			AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
			actionTimeout = 0;	//time out �� 0�̶� ���� �����̴� ���¶�°�.
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		}

		//��ǥ ������ �����ߴ°�.
		if (Lunia::Math::Length<float>(targetPosition - objectData.Position) < objectData.Radius) {
			//�����ߴٸ� �ٽ� ���ο��� ���ư���.
			ChangeState(*stateIdle, updateNormalTick);
		} else {
			idleCheckTime += AI::FSM::updateTick;
			if ( idleCheckTime > 10.0f ) { //1���� �ѵ��� ���� ��ǥ������ ���� ���ߴ�.
				ChangeState(*stateIdle, updateNormalTick);
			}

			uint32 dir = targetDirection[currentDirection];
			AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
			if (dir != targetDirection[currentDirection] ||
				(!CheckState(Database::Info::StateInfo::Type::MOVE)  && !CheckState(Database::Info::StateInfo::Type::STEP)) )
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
			actionTimeStamp = 0;
		}
	}

	void NonPlayer::FamiliarFightBegin()
	{
		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand)
			Command(Action::Action::CommandType::STOP, 0);
	}

	void NonPlayer::FamiliarFightUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"FamiliarFightUpdate()");
#endif

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj) ) {//����� �׾���. ���ο��� ���ư���.
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		float3 tempDirection = objectData.Direction;
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
		bufferedDirection = objectData.Direction;
		//bufferedDirection = AI::DirectionToFloat3(targetDirection[0]);

		//if ( ) //���̵��� ���� ����Ȯ�� üũ difficulty
		//{
			if (!DoAttack(obj)) objectData.Direction = tempDirection;
		//}

		ChangeState(stateCaution, updateNormalTick);
	}


	bool NonPlayer::DoAttack(Actor* target)
	{
		float prob=0;
		bool bSucceed = false;
		float r = stageData->GetRandomFloat();

		std::vector<Info::NonPlayerInfo::OffensiveSkillList::SkillList>* skillList;
		if ( NPCStatus.hpRate <= NPCStatus.npcInfo->OffensiveSkills.AggressiveMode ) //�������� ����� ������� ����Ѵ�.
			skillList = &NPCStatus.npcInfo->OffensiveSkills.AggressiveSkills;
		else
			skillList = &NPCStatus.npcInfo->OffensiveSkills.NormalSkills;

		//���� ������ �Ÿ��� ����ؼ� �ش� �Ÿ��� ��ų�� ������ش�.
		float distance = Lunia::Math::Length<float>(GetPosition() - target->GetPosition());

		std::vector<Info::NonPlayerInfo::OffensiveSkillList::SkillList>::iterator i = skillList->begin(), end = skillList->end();
		while ( i != end ) {
			if ( (*i).Range >= distance ) { //����� ��ų�� ��밡���� �Ÿ���.
				std::vector<Info::NonPlayerInfo::OffensiveSkillList::SkillList::Skill>::iterator i2 = (*i).Skills.begin(), end2 = (*i).Skills.end();
				for ( ;i2 != end2 ; ++i2 ) {
					prob += (*i2).Probability;
					if ( r <= prob ) {
						bSucceed = UseSkill( (*i2).Hash );
						return bSucceed;
					}
				}
			}
			++i;
		}

		//std::vector<Info::NonPlayerInfo::OffensiveSkill>::iterator skill = NPCStatus.npcInfo->.begin();
		//std::vector<Info::NonPlayerInfo::OffensiveSkill>::iterator end = NPCStatus.npcInfo->OffensiveSkills.end();
		//for (; skill != end ; ++skill) {
		//	prob += (*skill).Probability;
		//	if ( r <= prob ) {
		//		bSucceed = UseSkill((*skill).Hash);
		//		break;
		//	}
		//}
		return bSucceed;
	}
	Serial NonPlayer::SetTargetToNearest()
	{
		// ���� ����� ���� ã�´�.
		timeToNextReselectTarget = ReselectTargetInterval;				
		float minDistance =NPCStatus.sight;
		Serial nearest= NPCStatus.target;

		std::vector<Actor*> enemyList;
		// retrieve enemy list from stagedatabase
		stageData->GetAreaObjectForSector9(objectData.Position, NPCStatus.sight, Constants::AllianceType::ExTeam, characterData.Team, this, enemyList);

		if ( enemyList.empty() ) // no enemy in sight. find in hateList
		{
			return SetTargetToMostHate();
		}

		std::vector<Actor*>::const_iterator i = enemyList.begin();
		for( ; i != enemyList.end() ; ++i)
		{
			if( *i != NULL)
			{
				float distance = Lunia::Math::Length<float>(  (static_cast<Actor*>(*i))->GetPosition() - objectData.Position);
				if ( IsAbleToRecognize( *i ) && (*i)->GetType() == XRated::Constants::ObjectType::Player)
				{
					if( minDistance > distance )
					{
						minDistance = distance;
						nearest	= (static_cast<Actor*>(*i))->GetSerial();
					}
				}
			}

		}
		NPCStatus.target = nearest;
		
		return nearest;
	}

	Serial NonPlayer::SetTargetToMostHate()
	{
		std::map<Lunia::XRated::Serial, float>::iterator i = NPCStatus.hateList.begin();
		std::map<Lunia::XRated::Serial, float>::iterator end = NPCStatus.hateList.end();
		float dmg=0;
		for (; i != end ; ++i) {
			if ((*i).second > dmg) {
				dmg = (*i).second;
				NPCStatus.target = (*i).first;
			}
		}
		return NPCStatus.target;
	}

	const float NonPlayer::GetHitIgnoreProb( const float hitIgnoreValue ) const
	{
#ifdef _SINGLE
		return 100.0f;
#endif
		float hitIgnoreMultiplyValue0 = static_cast<float>((hitIgnoreValue + 8.f) / status.GetInt() - 1.f) * 100.f;
		float hitIgnoreMultiplyValue1 = hitIgnoreMultiplyValue0 > 0.f ? hitIgnoreMultiplyValue1 = 1.f : hitIgnoreMultiplyValue1 = -1.f;

		//float hitIgnoreMultiplyValue2 = pow(abs(hitIgnoreMultiplyValue0), 0.7f);
		//float hitIgnoreMultiplyValue3 = (hitIgnoreMultiplyValue1 * hitIgnoreMultiplyValue2  + 80.f)/ 100.f;
		float hitIgnoreMultiplyValue = 0.f;
		if (hitIgnoreMultiplyValue0 > 0.f)
			hitIgnoreMultiplyValue = (1.f * pow(abs(hitIgnoreMultiplyValue0), 0.7f) * 2.f + 80.f) / 100.f * hitIgnoreMultiply;
		else
			hitIgnoreMultiplyValue = (-1.f * pow(abs(hitIgnoreMultiplyValue0), 0.7f) + 80.f) / 100.f * hitIgnoreMultiply;

		//Lunia_IMPORTANT((L"hitIgnoreMultiplyValue0 = %f", hitIgnoreMultiplyValue0));
		//Lunia_IMPORTANT((L"hitIgnoreMultiplyValue1 = %f", hitIgnoreMultiplyValue1));
		//Lunia_IMPORTANT((L"hitIgnoreMultiplyValue2 = %f", hitIgnoreMultiplyValue2));
		//Lunia_IMPORTANT((L"hitIgnoreMultiplyValue3 = %f", hitIgnoreMultiplyValue3));
		//Lunia_IMPORTANT((L"hitIgnoreMultiplyValue = %f", hitIgnoreMultiplyValue));

		//float hitIgnoreMultiplyValue = ((hitIgnoreValue - status.Int) / 13.0f * 0.05f + 0.8f) * hitIgnoreMultiply;
		if (hitIgnoreMultiplyValue > 1.0f) hitIgnoreMultiplyValue = 1.0f;
		else if (hitIgnoreMultiplyValue < 0.f) hitIgnoreMultiplyValue = 0.f;

		if ( CheckState(Database::Info::StateInfo::Type::FLIED) )
		{
			hitIgnoreMultiplyValue *= 2.0f;
		}

		return hitIgnoreMultiplyValue;
	}

}	}	}
