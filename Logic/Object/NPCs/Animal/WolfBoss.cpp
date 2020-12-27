#include <Info/Info.h>
#include "WolfBoss.h"
#include "../../Player.h"
#include <Logic/GameState.h>
#include <Logic/AI/AIGlobalFunctions.h>

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	WolfBoss::WolfBoss()
	{
		triggerFlag = Trigger::TriggerType::DESTINATION_ARRIVED;
		cautionRange = 50.0f;
		approachRange = 65.0f;
		attackProbability = 0.04f;
	}

	bool WolfBoss::Update(float dt, IGameStateDatabase* db)
	{
		if (NonPlayer::Update(dt, db)) {
			stageData->AddTrigger(1, AI::Trigger::TriggerType::BOSS_DEAD, objectData.GameObjectSerial, objectData.Type,
										objectData.Position, 100, 1, false, false);
			return true;
		}
		return false;
	}

	void WolfBoss::stateCaution_SightUpdate()
	{
		//triggerFlag &= ~Trigger::TriggerType::NEED_COOPERATION;
		//집단행동
		if (stageData->GetRandomFloat() < 0.02f)
			ChangeState(stateCooperation, updateFightTick);
		else
			Wolf::stateCaution_SightUpdate();
	}

	void WolfBoss::stateCaution_HateListUpdate()
	{
		//집단행동
		if (stageData->GetRandomFloat() < 0.02f)
			ChangeState(stateCooperation, updateFightTick);
		else
			Wolf::stateCaution_HateListUpdate();
	}

	void WolfBoss::CooperationBegin()
	{
		SetAction( Database::Info::HashTable::Actions::Roar, NULL);
		waitTick = 2;
	}

	void WolfBoss::CooperationUpdate()
	{
		if (waitTick) {
			if (--waitTick == 0) {
				stageData->AddTrigger(1, AI::Trigger::TriggerType::NEED_COOPERATION, objectData.GameObjectSerial, objectData.Type,
										objectData.Position, 100, 10, false, false);
			}
		}

        if (GetActionNameHash() == Database::Info::HashTable::Actions::Roar)
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
			ChangeState(stateWarning, updateNormalTick);
			return;
		}
		float distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position);
		if ( distance > NPCStatus.attackRange ) {
			AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
			if (bCollide) {//충돌 상태인가.
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
