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

		if (status.Hp <= 0 && !IsState(stateEndOperation) ) {//�׾���.
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
		if (NPCStatus.target == 0) {//��� �������� ����. ������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Run )
			return;

		//������ �� �ִ°�.
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//������ �����ų� �׾��ų�, �þ߸� �����. hatelist ���� ���ش�.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0) {//HateList�� �����. ������
				targetPosition = obj->GetPosition();
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				SetDirection(AI::DirectionToFloat3(targetDirection[currentDirection]));
				Command(Action::Action::CommandType::ATTACKB, 0);
				ChangeState(stateWarning, updateNormalTick);
			}
			return;
		} else if (distance <= NPCStatus.attackRange) {//�̳��Ű ���ɷȾ�. ������
			ChangeState(stateCaution, updateNormalTick);
		}

		//�þ߾ȿ� ������ �����Ҹ��� �Ÿ��� �ƴϴ�. �ٰ�����.
		uint32 dir = targetDirection[currentDirection];
		targetPosition = obj->GetPosition();
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
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

	void Catapult::stateApproachTarget_HateListUpdate()
	{
		if (NPCStatus.target == 0) {//��� �������� ����. ������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		//if (GetActionName() != L"Stand" && GetActionName() != L"Run" && GetActionName() != L"Walk")
		//	return;

		//������ �� �ִ°�.
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		uint32 dir = targetDirection[currentDirection];
		targetPosition = obj->GetPosition();
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
		currentDirection = 0;

		if ( Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) <= NPCStatus.attackRange ) {//�̳��Ű ���ɷȾ�. ������
			SetDirection(AI::DirectionToFloat3(targetDirection[currentDirection]));
			Command(Action::Action::CommandType::ATTACKB, 0);
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		//���ݹ����� ��밡 ����. �̷���.. ������ �ٰ�����.

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

	void Catapult::stateCaution_HateListUpdate()
	{
		if (GetActionNameHash() == Database::Info::HashTable::Actions::Run) {
			Command(Action::Action::CommandType::STOP, 0);
			return;
		} else if (GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
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
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
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
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
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
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
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
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if (distance > 30.0f) {
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		if (bCollide) {//�浹 �����ΰ�.
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



