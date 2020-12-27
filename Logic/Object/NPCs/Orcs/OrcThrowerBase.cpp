#include <Info/Info.h>
#include "OrcThrowerBase.h"
#include "../../Player.h"
#include "../../../GameState.h"
#include <Core/Utils/Math/PseudoRandom.h>
#include "../../../AI/AIGlobalFunctions.h"

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	OrcThrowerBase::OrcThrowerBase() : NonPlayer(), stepBackCnt(0)
	{
		triggerFlag |= Trigger::TriggerType::NEED_HELP;
	}

	OrcThrowerBase::~OrcThrowerBase()
	{
	}

	Lunia::XRated::Serial OrcThrowerBase::RecalculateMostHateTarget()
	{
		Lunia::XRated::Serial currentTarget = NPCStatus.target;
		if (currentTarget != NonPlayer::RecalculateMostHateTarget() ) //대상이 바뀌었다.
			ChangeState(*stateApproachTarget, updateNormalTick);
		return NPCStatus.target;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation

	//FSMState
	void OrcThrowerBase::stateApproachTarget_SightUpdate()
	{
		if (NPCStatus.target == 0) {//까고 싶은놈이 없네. 경계모드로.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Run &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Walk)
			return;

		//공격할 수 있는가.
		//const float attackRange = 70;
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//게임을 나갔거나 죽었거나, 시야를 벗어났다. hatelist 에서 빼준다.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) < NPCStatus.sight) {
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0) {//HateList가 비었다. 경계모드로
				Command(Action::Action::CommandType::ATTACKA, targetDirection[currentDirection]);
				ChangeState(stateWarning, updateNormalTick);
			}
			return;
		} else if (distance <= NPCStatus.attackRange) {//이놈시키 딱걸렸어. 디져바
			ChangeState(stateCaution, updateNormalTick);
		}

		//시야안에 있지만 공격할만한 거리가 아니다. 다가가자.
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
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

	void OrcThrowerBase::stateApproachTarget_HateListUpdate()
	{
		if (NPCStatus.target == 0) {//까고 싶은놈이 없네. 경계모드로.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		//공격할 수 있는가.
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList가 비었다. 경계모드로
				ChangeState(stateWarning, updateNormalTick);
			return;
		}
		if ( Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) <= NPCStatus.attackRange ) {//이놈시키 딱걸렸어. 디져바
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		//공격범위에 상대가 없다. 이럴땐.. 가까이 다가가자.
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
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

	void OrcThrowerBase::stateCaution_HateListUpdate()
	{
		if (GetActionNameHash() == Database::Info::HashTable::Actions::Walk ||
			GetActionNameHash() == Database::Info::HashTable::Actions::Run) {
			Command(Action::Action::CommandType::STOP, 0);
			return;
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand)
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
		if ( distance <= NPCStatus.attackRange-20 ) {
			if (!bCollide) {
				if (stepBackCnt > 3) {
					stepBackCnt=0;
					ChangeState(*stateFightPtr, updateFightTick);
				} else {
					UseSkill( Database::Info::HashTable::Actions::StepBack);
				++stepBackCnt;
				}
				return;
			}
		} else if ( distance >= NPCStatus.attackRange ) {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return;
		}

		ChangeState(*stateFightPtr, updateFightTick);
	}

	void OrcThrowerBase::stateCaution_SightUpdate()
	{
		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList가 비었다. 경계모드로
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if ( Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) <= NPCStatus.attackRange-20 ) {
			if (!bCollide) {
				if (stepBackCnt > 3) {
					stepBackCnt=0;
					ChangeState(*stateFightPtr, updateFightTick);
				} else {
					UseSkill(Database::Info::HashTable::Actions::StepBack);
				++stepBackCnt;
				}
				return;
			}
		}

		ChangeState(*stateFightPtr, updateFightTick);
	}

	void OrcThrowerBase::FightBegin()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
	}

	void OrcThrowerBase::FightUpdate()
	{
		if (status.Hp <= 0)	{//웁스.. 죽엇다.
			ChangeState(stateEndOperation, updateNormalTick);
			return;
		}

		UseSkill( Database::Info::HashTable::Actions::Throw);
		ChangeState(stateCaution, updateNormalTick);
	}


}	}	}
