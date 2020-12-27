#include <Info/Info.h>
#include "Tower.h"
#include "../../Player.h"
#include <Logic/GameState.h>
#include <Logic/AI/AIGlobalFunctions.h>

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	Tower::Tower()
	{
		triggerFlag |= Trigger::TriggerType::NEED_HELP;
	}

	void Tower::Initialize(Lunia::Math::RandomInt& rInt)
	{
		SetAIInfo( Info::StageInfo::Npc::IdleStateType::STAND, NULL, float4(0,0,0,0), GetPosition(), Info::StageInfo::Npc::TransferType::SIGHT, 0);
		NonPlayer::Initialize(rInt);
	}

	bool Tower::DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
	{
		if (status.Hp <= 0)
			return false;
		return NonPlayer::DoDamage(who, creatorActionHash, minDmg, maxDmg, damage, dmgType, stateID, attackType, sFlag);
	}

	bool Tower::Update(float dt, IGameStateDatabase* db)
	{
		FSM::Update(dt);
		Actor::Update(dt, db);

		if (status.Hp <= 0 && !IsState(stateEndOperation) ) {//죽었다.
			ChangeState(stateEndOperation, updateNormalTick);
			return false;
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation

	//FSMState
	void Tower::IdleNoneBegin()
	{
	}

	void Tower::IdleNoneUpdate()
	{
		// tower doesn't call NonPlayer::IdleNoneUpdate function because it doesn't have any move actions.
		// So It' doesn't check position to return
		// It should call NonPlayer::CheckSurroundEnemy() function instead.
		CheckSurroundEnemy();
	}

	void Tower::WarningBegin()
	{
	}

	void Tower::stateApproachTarget_SightUpdate()
	{
		if (NPCStatus.target == 0) {//까고 싶은놈이 없네. 경계모드로.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand)
			return;

		//공격할 수 있는가.
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//게임을 나갔거나 죽었거나, 시야를 벗어났다. hatelist 에서 빼준다.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0) {//HateList가 비었다. 경계모드로
				ChangeState(stateWarning, updateNormalTick);
			}
			return;
		} else
			ChangeState(*stateFightPtr, updateFightTick);
	}

	void Tower::stateCaution_SightUpdate()
	{
		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || obj->CheckState(Info::StateInfo::Type::GHOST)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				//HateList가 비었다. 경계모드로
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);

		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance >= NPCStatus.attackRange ) {
			ChangeState(*stateIdle, updateNormalTick);
			return;
		}

		ChangeState(*stateFightPtr, updateFightTick);
	}

	void Tower::FightUpdate()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			ChangeState(*stateIdle, updateNormalTick);
			return;
		}
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);

		DoAttack(obj);

		ChangeState(stateCaution, updateNormalTick);
	}

	void Tower::EndOperationBegin()
	{
		SetAction(Database::Info::HashTable::Actions::Special01, NULL, true);
	}

}	}	}



