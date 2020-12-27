#include <Info/Info.h>
#include "OrcWarriorBase.h"
#include "../../Player.h"
#include "../../../GameState.h"
#include "../../../AI/AIGlobalFunctions.h"

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	OrcWarriorBase::OrcWarriorBase() : NonPlayer(), minimumRange(20.0f)
	{
		triggerFlag |= Trigger::TriggerType::NEED_HELP;
	}

	Lunia::XRated::Serial OrcWarriorBase::RecalculateMostHateTarget()
	{
		Lunia::XRated::Serial currentTarget = NPCStatus.target;
		if (currentTarget != NonPlayer::RecalculateMostHateTarget() ) //����� �ٲ����.
			ChangeState(*stateApproachTarget, updateNormalTick);
		return NPCStatus.target;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation

	//FSMState
	void OrcWarriorBase::stateApproachTarget_SightUpdate()
	{
		NonPlayer::stateApproachTarget_SightUpdate();
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( obj && Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) < NPCStatus.attackRange) {
			SetDirection(AI::DirectionToFloat3(targetDirection[currentDirection]));
			Command(Action::Action::CommandType::ATTACKB, 0);
			ChangeState(stateCaution, updateNormalTick);
		}
	}

	void OrcWarriorBase::stateApproachTarget_HateListUpdate()
	{
		NonPlayer::stateApproachTarget_HateListUpdate();

		// TODO : ���� ����� �Ǿ� ��������.. ��¿ �� ����.
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( obj && Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position) < NPCStatus.attackRange) {
			SetDirection(AI::DirectionToFloat3(targetDirection[currentDirection]));
			Command(Action::Action::CommandType::ATTACKB, 0);
			ChangeState(stateCaution, updateNormalTick);
		}
	}

	void OrcWarriorBase::stateCaution_HateListUpdate()
	{
		if (GetActionNameHash() == Database::Info::HashTable::Actions::Walk ||
			GetActionNameHash() == Database::Info::HashTable::Actions::Run) {
			Command(Action::Action::CommandType::STOP, 0);
			return;
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

		//uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance <= NPCStatus.attackRange ) {
			objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
			if ( stageData->GetRandomFloat() < 0.55f )
				UseSkill( Database::Info::HashTable::Actions::StepBack);
			else
				UseSkill(Database::Info::HashTable::Actions::UpperAtk);
			return;
		} else {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return;
		}

		//float r = stageData->GetRandomFloat();
		//if(r < 0.2f) // 1/5�� Ȯ���� �����Ѵ�.
		//	ChangeState(*stateFightPtr);
		//else if (r < 0.4f) {
		//	objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
		//	UseSkill( Database::Info::HashTable::Actions::Roar);
		//} else {
		//	if (bCollide)
		//		sideStep = 1-sideStep;
		//	
		//	if( r < 0.1f )
		//	{
		//		ChangeState( stateCaution );
		//		return;
		//	}

		//	if ( GetActionNameHash() == Database::Info::HashTable::Actions::Stand || dir != targetDirection[0] || bCollide ) {
		//		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
		//		SetAction( sideStep ? Database::Info::HashTable::Actions::StepRight : Database::Info::HashTable::Actions::StepLeft, NULL);
		//	}
		//}
	}

	void OrcWarriorBase::stateCaution_SightUpdate()
	{
		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::StepLeft &&
			GetActionNameHash() != Database::Info::HashTable::Actions::StepRight)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		float length;
		if ( obj )
			length = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		else
			length = 0;

		if (!obj || !IsAbleToRecognize(obj) ||
			obj->CheckState(Info::StateInfo::Type::INVISIBLE) || length > NPCStatus.sight) {//���� ����� ����
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList�� �����. ������
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);

		if ( length <= minimumRange ) {
			objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
			UseSkill( Database::Info::HashTable::Actions::StepBack);
			return;
		} else if ( length >= 90 ) {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return;
		}


		float r = stageData->GetRandomFloat();
		if(r < 0.2f) // 1/3�� Ȯ���� �����Ѵ�.
			ChangeState(*stateFightPtr, updateFightTick);
		else if (r < 0.4f) {
			objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
			UseSkill( Database::Info::HashTable::Actions::Roar);
		} else {
			if (bCollide)
				sideStep = 1-sideStep;

			if ( GetActionName() == L"Stand" || dir != targetDirection[0] || bCollide ) {
				objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
				SetAction( sideStep ? Database::Info::HashTable::Actions::StepRight : Database::Info::HashTable::Actions::StepLeft, NULL);
			}
		}
	}

	void OrcWarriorBase::FightUpdate()
	{
#ifdef _SINGLE
		if ( stageData->showNPCInfo == 3 )
			stageData->NPCDebugMessage(GetSerial(), L"FightUpdate()");
#endif

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//�󾾱�.. �̳� �׾�������. ���� ����� ��������.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
		bufferedDirection = objectData.Direction;

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if (distance <= minimumRange)
			UseSkill( Database::Info::HashTable::Actions::UpperAtk);
		else {
			DoAttack(obj);
		}
		ChangeState(stateCaution, updateNormalTick);
	}

}	}	}
