#include <Info/Info.h>
#include "Wolf.h"
#include "../../Player.h"
#include <Logic/GameState.h>
#include <Logic/AI/AIGlobalFunctions.h>

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	Wolf::Wolf()
	{
		triggerFlag |= Trigger::TriggerType::NEED_COOPERATION | Trigger::TriggerType::BOSS_DEAD;

		cautionRange = 40.0f;
		approachRange = 55.0f;
		attackProbability = 0.07f;
	}

	bool Wolf::HandleTrigger(AI::Trigger& trigger)
	{
		switch (trigger.type) {
		case Trigger::TriggerType::NEED_COOPERATION :
			{
				NonPlayer* npc= stageData->RetrieveNPC( trigger.creator );
				if (npc) {
					Serial serial = npc->GetMostHate();
					ClearHateList();
					ChangeHate(serial, 100);
				}
				ChangeState(stateCooperation, updateFightTick);
				trigger.bExpired = true;
				return true;
			}
		case Trigger::TriggerType::BOSS_DEAD :
			ChangeState(stateCooperation, updateFightTick);
			//SetAction(L"Roar", NULL, true);
			trigger.bExpired = true;
			return true;
		}
		return NonPlayer::HandleTrigger(trigger);
	}

	void Wolf::stateApproachTarget_SightUpdate()
	{
		if (NPCStatus.target == 0) {//��� �������� ����. ������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		//������ �� �ִ°�.
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//������ �����ų� �׾��ų�, �þ߸� �����. hatelist ���� ���ش�.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {
			RemoveHateTarget(NPCStatus.target);
			ChangeState(*stateIdle, updateNormalTick);
			return;
		} else if (distance <= cautionRange) {//�ٰ����� �Ÿ��� �ΰ� ƴ�� �븮��
			ChangeState(stateCaution, updateNormalTick);
		}

		//�þ߾ȿ� ������ ��ġ �Ÿ��� �ƴϴ�. �ٰ�����.
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		currentDirection = 0;

		if (bCollide) {//�浹 �����ΰ�.
			++collisionCnt;
			if (collisionCnt == 6)
				collisionCnt = 0;
			currentDirection = collisionCnt;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else collisionCnt = 0;

		if (dir != targetDirection[currentDirection] || GetActionName() == L"Stand")
			Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);
	}

	void Wolf::stateApproachTarget_HateListUpdate()
	{
		//�ڽ��� ó�� ��ġ���� ���� ����°��� �˻��Ѵ�.
		if ( Lunia::Math::Length<float>(objectData.Position - targetPosition) > NPCStatus.sight*2) //�ڽ��� ������ ��ġ���� �־����� hate�� �پ���.
			ReduceHate(characterData.MaxHp/30.0f);

		if (NPCStatus.target == 0) {//��� �������� ����. ������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		//������ �� �ִ°�.
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
				ChangeState(stateWarning, updateNormalTick);
			return;
		}
		if ( Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) <= cautionRange ) {
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		//�þ߾ȿ� ������ �����Ҹ��� �Ÿ��� �ƴϴ�. �ٰ�����.
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		currentDirection = 0;

		if (bCollide) {//�浹 �����ΰ�.
			++collisionCnt;
			if (collisionCnt == 6)
				collisionCnt = 0;
			currentDirection = collisionCnt;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else collisionCnt = 0;

		if (dir != targetDirection[currentDirection] || GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
			Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);
	}

	void Wolf::stateCaution_SightBegin()
	{
		if (GetCurrentAction()->GetActionClassType() == ActionInfo::ActionClassType::MOVE) {
			Command(Action::Action::CommandType::STOP, 0);
			return;
		}
	}

	void Wolf::stateCaution_SightUpdate()
	{
		if (GetCurrentAction()->GetActionClassType() == ActionInfo::ActionClassType::MOVE) {
			//���Ƽ� �������� ���̰���
			Command(Action::Action::CommandType::STOP, 0);
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::StepLeft &&
			GetActionNameHash() != Database::Info::HashTable::Actions::StepRight)
			return;
	
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		float distance = 0;
		if (obj)
			distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if (!obj || !IsAbleToRecognize(obj) || distance > NPCStatus.sight) {//���� ����� ����
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if ( distance <= NPCStatus.attackRange ) {
			if ( stageData->GetRandomFloat() < 0.5f )
				ChangeState(*stateFightPtr, updateFightTick);
			else {//�ƴ϶�� ��������.
				AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection, true);
				//objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
				Command(Action::Action::CommandType::DASH, targetDirection[0]);
			}
			return;
		} else if ( distance > approachRange ) {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return;
		}

		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);

		float r = stageData->GetRandomFloat();
		if(r < attackProbability) { // 1/3�� Ȯ���� �����Ѵ�.
			objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
			Command(Action::Action::CommandType::STOP, 0);
			ChangeState(*stateFightPtr, updateFightTick);
		} else if (r < 0.15f) {
			UseSkill( Database::Info::HashTable::Actions::Bark);
		} else {
			if (bCollide)
				sideStep = 1-sideStep;

			if ( GetActionNameHash() == Database::Info::HashTable::Actions::Stand || dir != targetDirection[0] || bCollide ) {
				objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
				SetAction( sideStep ? Database::Info::HashTable::Actions::StepRight : Database::Info::HashTable::Actions::StepLeft, NULL);
			}
		}
	}

	void Wolf::stateCaution_HateListBegin()
	{
		if (GetCurrentAction()->GetActionClassType() == ActionInfo::ActionClassType::MOVE) {
			Command(Action::Action::CommandType::STOP, 0);
			return;
		}
	}

	void Wolf::stateCaution_HateListUpdate()
	{
		if (GetCurrentAction()->GetActionClassType() == ActionInfo::ActionClassType::MOVE) {
			//���Ƽ� �������� ���̰���
			Command(Action::Action::CommandType::STOP, 0);
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::StepLeft &&
			GetActionNameHash() != Database::Info::HashTable::Actions::StepRight)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance <= NPCStatus.attackRange ) {
			if ( stageData->GetRandomFloat() < 0.5f )
				ChangeState(*stateFightPtr, updateFightTick);
			else {//�ƴ϶�� ��������.
				AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection, true);
				//objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
				Command(Action::Action::CommandType::DASH, targetDirection[0]);
			}
			return;
		} else if ( distance > approachRange ) {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return;
		}

		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);

		float r = stageData->GetRandomFloat();
		if(stageData->GetRandomFloat() < attackProbability) {
			objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
			Command(Action::Action::CommandType::STOP, 0);
			ChangeState(*stateFightPtr, updateFightTick);
		} else if (r < 0.15f) {
			SetAction( Database::Info::HashTable::Actions::Bark, NULL);
		} else {
			if (bCollide)
				sideStep = 1-sideStep;

			if ( GetActionNameHash() == Database::Info::HashTable::Actions::Stand || dir != targetDirection[0] || bCollide ) {
				objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
				SetAction( sideStep ? Database::Info::HashTable::Actions::StepRight : Database::Info::HashTable::Actions::StepLeft, NULL);
			}
		}
	}

	void Wolf::FightBegin()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance > NPCStatus.attackRange ) {
			AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
			Command(Action::Action::CommandType::DASH, targetDirection[0]);
		}
	}

	void Wolf::FightUpdate()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance > NPCStatus.attackRange ) {
			AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
			if (bCollide) {//�浹 �����ΰ�.
				++collisionCnt;
				if (collisionCnt == 6)
					collisionCnt = 0;
				currentDirection = collisionCnt;
				Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);
			} else {
				collisionCnt = 0;
				Command(Action::Action::CommandType::DASH, targetDirection[0]);
			}
		} else {
			NonPlayer::FightUpdate();
		}
	}

	void Wolf::CooperationBegin()
	{
		if ( !SetAction( Database::Info::HashTable::Actions::Roar, NULL) ) { //� ������ ���� ������ �����ߴ�.
			ChangeState(stateWarning, updateNormalTick);
		}
		SetHeight(0);
		waitTick = stageData->GetRandomInt(1, 6);
	}

	void Wolf::CooperationUpdate()
	{
		if (waitTick) {
			--waitTick;
			return;
		}
        if (GetActionNameHash() == Database::Info::HashTable::Actions::Roar)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance > NPCStatus.attackRange ) {
			AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
			if (bCollide) {//�浹 �����ΰ�.
				++collisionCnt;
				if (collisionCnt == 6)
					collisionCnt = 0;
				currentDirection = collisionCnt;
				Command(Action::Action::CommandType::DASH, targetDirection[currentDirection]);
			} else {
				collisionCnt = 0;
				Command(Action::Action::CommandType::DASH, targetDirection[0]);
			}
		} else {
			AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
			objectData.Direction = bufferedDirection = AI::DirectionToFloat3(targetDirection[0]);
			Command(Action::Action::CommandType::ATTACKA, 0);
			ChangeState(stateCaution, updateNormalTick);
		}
	}

}	}	}
