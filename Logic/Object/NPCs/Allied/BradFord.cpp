#include <Info/Info.h>
#include "BradFord.h"
#include "../../../GameState.h"
#include "../../../AI/AIGlobalFunctions.h"

using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	BradFord::BradFord()
	{
		triggerFlag = Trigger::TriggerType::DESTINATION_ARRIVED;
	}

	void BradFord::IdleNoneBegin()
	{
		Command(Action::Action::CommandType::ATTACKA, 0);
	}

	void BradFord::IdleNoneUpdate()
	{
	}

	void BradFord::IdleNoneEnd()
	{
	}

	void BradFord::IdlePathFindingBegin()
	{
		ClearHateList();

		std::vector<float2>& path = NPCStatus.paths->at(NPCStatus.pathCnt);
		std::vector<float2>::iterator i = path.begin(), end = path.end();
		float2 sourcePos( objectData.Position.x, objectData.Position.z );
		float2 targetPos, currentPos;
		float2 direction;
		for (uint32 posCnt=0; i != end ; ++i, ++posCnt) {
            targetPos = (*i);
			currentPos = sourcePos;
			float len = sqrt( pow(targetPos.x-sourcePos.x, 2) + pow(targetPos.y-sourcePos.y, 2) );
			direction.SetVector( (targetPos.x-sourcePos.x)/len, (targetPos.y-sourcePos.y)/len );
			uint32 cnt = (uint32)len+2;
			while(--cnt) {
				currentPos += direction;
				if (!stageData->IsAbletoLocateInMap(float3(currentPos.x, 0, currentPos.y), objectData.Radius))
					break;
			}
			if (!cnt) {
				targetPosition.SetVector(targetPos.x, 0, targetPos.y);
				this->posCnt = posCnt;
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				currentDirection = collisionCnt = 0;
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
				return;
			}
		}
	}

	void BradFord::IdlePathFindingUpdate()
	{
		if (Lunia::Math::Length<float>(targetPosition - objectData.Position) < objectData.Radius) {
			if ( ++posCnt < ( NPCStatus.paths->at(NPCStatus.pathCnt) ).size() ) {
				targetPosition.SetVector( (*NPCStatus.paths)[NPCStatus.pathCnt][posCnt].x, 0, (*NPCStatus.paths)[NPCStatus.pathCnt][posCnt].y );
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				currentDirection = collisionCnt = 0;
				actionTimeout = 1.5f;
				actionTimeStamp = 0;
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
			} else {
				stateIdle = &stateIdleNone;
				ChangeState(*stateIdle, updateNormalTick);
				return;
			}
		}
		uint32 dir = targetDirection[currentDirection];
		AI::DecideDirection(objectData.Position, targetPosition, targetDirection);

		if (bCollide) {
			++collisionCnt;
			if (collisionCnt == 6)
				collisionCnt = 0;
			currentDirection = collisionCnt;
			Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		} else {
			collisionCnt = 0;
			currentDirection = collisionCnt;
			if (dir != targetDirection[currentDirection])
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
		}
	}

	void BradFord::IdlePathFindingEnd()
	{
	}

}	}	}

