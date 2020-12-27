#include <Info/Info.h>
#include "Catapult.h"
#include "../../Player.h"
#include <Logic/GameState.h>
#include <Logic/AI/AIGlobalFunctions.h>

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	Catapult::Catapult() : minimumDistance(30.0f)
	{
		triggerFlag |= Trigger::TriggerType::NEED_HELP;
	}

	void Catapult::Initialize(Lunia::Math::RandomInt& rInt)
	{
		stateRetreat.Set(L"retreat", this, &NonPlayer::stateRetreat_Begin, &NonPlayer::stateRetreat_Update, &NonPlayer::stateRetreat_End);

		NonPlayer::Initialize(rInt);
	}

	bool Catapult::DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
	{
		if (status.Hp <= 0)
			return false;
		return NonPlayer::DoDamage(who, creatorActionHash, minDmg, maxDmg, damage, dmgType, stateID, attackType, sFlag);
	}

	bool Catapult::Update(float dt, IGameStateDatabase* db)
	{
		FSM::Update(dt);
		Actor::Update(dt, db);

		if (status.Hp <= 0 && !IsState(stateEndOperation) ) {//죽었다.
			ChangeState(stateEndOperation, updateNormalTick);
			return false;
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation
	bool Catapult::HandleTrigger(AI::Trigger& trigger)
	{
		return NonPlayer::HandleTrigger(trigger);
	}

	//FSMState
	void Catapult::stateApproachTarget_SightUpdate()
	{
		if (NPCStatus.target == 0) {//까고 싶은놈이 없네. 경계모드로.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Run )
			return;

		//공격할 수 있는가.
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//게임을 나갔거나 죽었거나, 시야를 벗어났다. hatelist 에서 빼준다.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0) {//HateList가 비었다. 경계모드로
				targetPosition = obj->GetPosition();
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				SetDirection(AI::DirectionToFloat3(targetDirection[currentDirection]));
				Command(Action::Action::CommandType::ATTACKB, 0);
				ChangeState(stateWarning, updateNormalTick);
			}
			return;
		} else if (distance <= NPCStatus.attackRange) {//이놈시키 딱걸렸어. 디져바
			ChangeState(stateCaution, updateNormalTick);
		}

		//시야안에 있지만 공격할만한 거리가 아니다. 다가가자.
		uint32 dir = targetDirection[currentDirection];
		targetPosition = obj->GetPosition();
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
		currentDirection = 0;

		if (bCollide) {//충돌 상태인가.
			++collisionCnt;
			if (collisionCnt == 6)
				collisionCnt = 0;
			currentDirection = collisionCnt;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else collisionCnt = 0;

		if (dir != targetDirection[currentDirection] || GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
			Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);
	}

	void Catapult::stateApproachTarget_HateListUpdate()
	{
		if (NPCStatus.target == 0) {//까고 싶은놈이 없네. 경계모드로.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		//if (GetActionName() != L"Stand" && GetActionName() != L"Run" && GetActionName() != L"Walk")
		//	return;

		//공격할 수 있는가.
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList가 비었다. 경계모드로
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		uint32 dir = targetDirection[currentDirection];
		targetPosition = obj->GetPosition();
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
		currentDirection = 0;

		if ( Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) <= NPCStatus.attackRange ) {//이놈시키 딱걸렸어. 디져바
			SetDirection(AI::DirectionToFloat3(targetDirection[currentDirection]));
			Command(Action::Action::CommandType::ATTACKB, 0);
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		//공격범위에 상대가 없다. 이럴땐.. 가까이 다가가자.

		if (bCollide) {//충돌 상태인가.
			++collisionCnt;
			if (collisionCnt == 6)
				collisionCnt = 0;
			currentDirection = collisionCnt;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else collisionCnt = 0;

		if (dir != targetDirection[currentDirection] || GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
			Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);
	}

	void Catapult::stateCaution_HateListUpdate()
	{
		if (GetActionNameHash() == Database::Info::HashTable::Actions::Run) {
			Command(Action::Action::CommandType::STOP, 0);
			return;
		} else if (GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList가 비었다. 경계모드로
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance <= minimumDistance ) {
			ChangeState(stateRetreat, updateNormalTick);
			return;
		} else if ( distance >= NPCStatus.attackRange ) {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return;
		}

		ChangeState(*stateFightPtr, updateFightTick);
	}

	void Catapult::stateCaution_SightUpdate()
	{
		if (GetActionNameHash() == Database::Info::HashTable::Actions::Run) {
			Command(Action::Action::CommandType::STOP, 0);
			return;
		} else if (GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList가 비었다. 경계모드로
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance <= minimumDistance ) {
			ChangeState(stateRetreat, updateNormalTick);
			return;
		} else if ( distance >= NPCStatus.attackRange ) {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return;
		}

		ChangeState(*stateFightPtr, updateFightTick);
	}

	void Catapult::FightUpdate()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if (distance <= minimumDistance) {
			ChangeState(stateRetreat, updateNormalTick);
			return;
		} else {
			DoAttack(obj);
		}
		ChangeState(stateCaution, updateNormalTick);
	}

	void Catapult::stateRetreat_Begin()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection, true);
		Command( Action::Action::CommandType::MOVE, targetDirection[0] );
		collisionCnt = 0;
	}

	void Catapult::stateRetreat_Update()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if (distance > 30.0f) {
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		if (bCollide) {//충돌 상태인가.
			++collisionCnt;
			if (collisionCnt == 6)
				collisionCnt = 0;
			currentDirection = collisionCnt;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		}
	}

	void Catapult::stateRetreat_End()
	{
		Command(Action::Action::CommandType::STOP, 0);
	}

	void Catapult::EndOperationBegin()
	{
		SetAction(Database::Info::HashTable::Actions::Special01, NULL, true);
	}

}	}	}



