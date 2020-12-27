#include <Info/Info.h>
#include "Mimic.h"
#include "../../Player.h"
#include <Logic/GameState.h>
#include <Logic/AI/AIGlobalFunctions.h>

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	Mimic::Mimic()
	{
		triggerFlag = Trigger::TriggerType::DESTINATION_ARRIVED;
	}

	void Mimic::Initialize(Lunia::Math::RandomInt& rInt)
	{
		SetAIInfo( StageInfo::Npc::IdleStateType::STAND, NULL, float4(0,0,0,0), GetPosition(), StageInfo::Npc::TransferType::HATELIST, 0);
		NonPlayer::Initialize(rInt);
	}

	bool Mimic::DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
	{
		//미믹은 밀리어택 이외에는 데미지를 입지 않는다.
		if ( who == NULL || who->GetType() == Constants::ObjectType::Projectile/* || dmgType != Constants::DamageType::PHYSICAL*/)
			return false;
		if ( NonPlayer::DoDamage(who, creatorActionHash, minDmg, maxDmg, damage, dmgType, stateID, attackType, sFlag) ) {
			ChangeState(*stateApproachTarget, updateNormalTick);
			return true;
		} else
			return false;
	}

	bool Mimic::Update(float dt, IGameStateDatabase* db)
	{
		FSM::Update(dt);
		Actor::Update(dt, db);

		if (status.Hp <= 0) {//죽었다.
			return true;
		}

		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation
	bool Mimic::HandleTrigger(AI::Trigger& trigger)
	{
		return NonPlayer::HandleTrigger(trigger);
	}

	void Mimic::IdleNoneBegin()
	{
		SetAction(Database::Info::HashTable::Actions::Special01, NULL, true);
		float distance = Lunia::Math::Length<float>(initialPos - objectData.Position);
		if ( distance > objectData.Radius ) {//처음 위치에서 벗어나 있다.
			targetPosition = initialPos;
			AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
			currentDirection = collisionCnt = 0;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		}
	}

	void Mimic::IdleNoneUpdate()
	{
		if (GetActionNameHash() == Database::Info::HashTable::Actions::Special01)
			return;

		float distance = Lunia::Math::Length<float>(initialPos - objectData.Position);
		if ( distance < objectData.Radius ) {//처음 위치에서 벗어나 있다.
			objectData.Direction = Constants::DirectionF::Down;
			SetAction(Database::Info::HashTable::Actions::Special01, NULL, true);
			return;
		}
		NonPlayer::IdleNoneUpdate();
	}

	void Mimic::IdleNoneEnd()
	{
		NonPlayer::IdleNoneEnd();
	}

	void Mimic::FightUpdate()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);

		DoAttack(obj);
		ChangeState(stateCaution, updateNormalTick);
	}

}	}	}



