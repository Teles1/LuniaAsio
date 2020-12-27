#include <Info/Info.h>
#include "Slime.h"
#include <Logic/GameState.h>
#include "../../../AI/AIGlobalFunctions.h"

using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	SlimeBoss::SlimeBoss() :bSplitted(false)
	{
	}

	bool SlimeBoss::DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
	{
		std::wstring actionName = GetActionName();
		if ( NonPlayer::DoDamage(who, creatorActionHash, minDmg, maxDmg, damage, dmgType, stateID, attackType, sFlag) ) {
			++hitCnt;
			if (actionName == L"Spell02" && hitCnt == 10) {
				Logger::GetInstance().Info( L"[SlimeBoss::DoDamage] Special attack canceled." );
				SetAction(GetNPCInfo()->StartActionHash, NULL);
			}
			return true;
		}
		return false;
	}

	void SlimeBoss::FightUpdate()
	{
		if ( GetActionNameHash() != Database::Info::HashTable::Actions::Stand )
			return;

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		float distance;
		if ( !obj || !IsAbleToRecognize(obj) ) {
			RemoveHateTarget(NPCStatus.target);
			ChangeState(*stateIdle, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);
		bufferedDirection = objectData.Direction;

		if ( ( distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) < 30 ) {
			UseSkill( Database::Info::HashTable::Actions::Atk01);
		} else
			NonPlayer::FightUpdate();
	}

}	}	}



