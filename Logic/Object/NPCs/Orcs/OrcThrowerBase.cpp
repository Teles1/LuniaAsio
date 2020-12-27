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
		if (currentTarget != NonPlayer::RecalculateMostHateTarget() ) //����� �ٲ����.
			ChangeState(*stateApproachTarget, updateNormalTick);
		return NPCStatus.target;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation

	//FSMState
	void OrcThrowerBase::stateApproachTarget_SightUpdate()
	{
		if (NPCStatus.target == 0) {//��� �������� ����. ������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Run &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Walk)
			return;

		//������ �� �ִ°�.
		//const float attackRange = 70;
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//������ �����ų� �׾��ų�, �þ߸� �����. hatelist ���� ���ش�.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) < NPCStatus.sight) {
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0) {//HateList�� �����. ������
				Command(Action::Action::CommandType::ATTACKA, targetDirection[currentDirection]);
				ChangeState(stateWarning, updateNormalTick);
			}
			return;
		} else if (distance <= NPCStatus.attackRange) {//�̳��Ű ���ɷȾ�. ������
			ChangeState(stateCaution, updateNormalTick);
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

	void OrcThrowerBase::stateApproachTarget_HateListUpdate()
	{
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
		if ( Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) <= NPCStatus.attackRange ) {//�̳��Ű ���ɷȾ�. ������
			ChangeState(stateCaution, updateNormalTick);
			return;
		}

		//���ݹ����� ��밡 ����. �̷���.. ������ �ٰ�����.
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
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
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
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
	}

	void OrcThrowerBase::FightUpdate()
	{
		if (status.Hp <= 0)	{//��.. �׾���.
			ChangeState(stateEndOperation, updateNormalTick);
			return;
		}

		UseSkill( Database::Info::HashTable::Actions::Throw);
		ChangeState(stateCaution, updateNormalTick);
	}


}	}	}
