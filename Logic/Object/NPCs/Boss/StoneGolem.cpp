#include <Info/Info.h>
#include "StoneGolem.h"
#include "../../Player.h"
#include "../../../GameState.h"
#include "../../../AI/AIGlobalFunctions.h"

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	StoneGolem::StoneGolem() : NonPlayer()
	{
		characterData.Team = 3;
	}

	StoneGolem::~StoneGolem()
	{
	}

	void StoneGolem::Initialize(Lunia::Math::RandomInt& rInt)
	{
		NonPlayer::Initialize(rInt);

		//AI FSMState define
		stateDead.Set(L"dead", this, &StoneGolem::DeadBegin, &StoneGolem::DeadUpdate, &StoneGolem::DeadEnd);
		stateCrazy.Set(L"crazy", this, &StoneGolem::CrazyBegin, &StoneGolem::CrazyUpdate, &StoneGolem::CrazyEnd);
	}

	bool StoneGolem::DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
	{
		std::wstring actionName = GetActionName();
		if (actionName == L"StartUp")
			return false;
		if ( NonPlayer::DoDamage(who, creatorActionHash, minDmg, maxDmg, damage, dmgType, stateID, attackType, sFlag) ) {
			++hitCnt;
			if (actionName == L"SkillAtk1" && hitCnt == stageData->GetPlayerCnt()*20 ) {
				Logger::GetInstance().Info (L"[StoneGolem::DoDamage] Special attack canceled.");
                SetAction(3490595, NULL, true);	// L"DeathBlowCanceled"
			} else {
				;//Lunia_INFO(( L"[StoneGolem::DoDamage] hitCnt [{}], Hp [%f/%f]", hitCnt, characterData.Hp, characterData.MaxHp));
			}

			float max;
			if (GetHP(max) < max/2.0f && !IsState(stateCrazy) )
				ChangeState(stateCrazy, updateNormalTick);

			return true;
		}
		return false;
	}

	Serial StoneGolem::RecalculateMostHateTarget()
	{
		Serial currentTarget = NPCStatus.target;
		if (currentTarget != NonPlayer::RecalculateMostHateTarget() ) //대상이 바뀌었다.
			ChangeState(*stateApproachTarget, updateNormalTick);
		return NPCStatus.target;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AI Implementation
	void StoneGolem::IdleNoneBegin()
	{
		ClearHateList();
		actions.SetAction( 613318, this, NULL, *stageData, true); //L"Rock"
	}

	void StoneGolem::IdleNoneEnd()
	{
		actions.SetAction( 36390304, this, NULL, *stageData); // L"StartUp"
	}

	void StoneGolem::stateApproachTarget_SightBegin()
	{
		currentDirection = 0;
	}

	void StoneGolem::stateApproachTarget_SightUpdate()
	{
		if (NPCStatus.target == 0) {
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		if (GetActionNameHash() != Database::Info::HashTable::Actions::Stand &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Walk &&
			GetActionNameHash() != Database::Info::HashTable::Actions::Run)
			return;
		if (stageData->GetRandomFloat() < 0.05f) {
			UseSkill( Database::Info::HashTable::Actions::Roar);
			return;
		}

		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj) ) {
			RemoveHateTarget(NPCStatus.target);
			if (NPCStatus.target == 0)
				ChangeState(stateWarning, updateNormalTick);
			return;
		}

		float length = Lunia::Math::Length<float>( float2(objectData.Position.x-obj->GetPosition().x, objectData.Position.z-obj->GetPosition().z) );
		if (length < NPCStatus.attackRange) {
			ChangeState(*stateFightPtr, updateFightTick);
		} else {
			uint32 dir = targetDirection[currentDirection];
			targetPosition = obj->GetPosition();
			AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
			currentDirection = 0;
			if (dir != targetDirection[currentDirection])
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);

			return;
		}

	}

	void StoneGolem::FightBegin()
	{
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if (!obj || !IsAbleToRecognize(obj)) {
			ChangeState(stateWarning, updateNormalTick);
			return;
		}

		AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		objectData.Direction = AI::DirectionToFloat3(targetDirection[0]);

		hitCnt = 0;
		float r = stageData->GetRandomFloat();
		if(r < 0.25f)
			actions.SetAction(111928, this, NULL, *stageData); // L"Atk1"
		else if(r < 0.5f)
			actions.SetAction(6484100, this, NULL, *stageData); // L"Atk2End"
		else if(r < 0.75f)
			actions.SetAction(111930, this, NULL, *stageData); // L"Atk3"
		else
			actions.SetAction(Database::Info::HashTable::Actions::DownAtk, this, NULL, *stageData);
		return;
	}

	void StoneGolem::FightUpdate()
	{
		if (GetActionNameHash() == Database::Info::HashTable::Actions::Stand ||
			GetActionNameHash() == Database::Info::HashTable::Actions::Walk ||
			GetActionNameHash() == Database::Info::HashTable::Actions::Run) {
			//공격이 풀리고 다가갈수 있게 됐다면, 혹은 다가가는 중이라면

			Actor* obj = stageData->RetrieveActor(NPCStatus.target);
			if (!obj || !IsAbleToRecognize(obj)) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
				ChangeState(stateWarning, updateNormalTick);
				return;
			}

			//공격할 수 있는가.
			float length = Lunia::Math::Length<float>(float2(objectData.Position.x-obj->GetPosition().x, objectData.Position.z-obj->GetPosition().z));

			if (length < NPCStatus.attackRange) {
				if (stageData->GetRandomFloat() >= 0.5f) //매 틱 1/2 의 확률로 공격 작동.
					return;

				float r = stageData->GetRandomFloat();
				if ( r <= 0.333333f)	// 1/3 의 확률씩으로 각각의 액션을 취해주자.
					UseSkill(111928); // L"Atk1"
				else if ( r <= 0.666666f )
					UseSkill(6484100); // L"Atk2End"
				else
					UseSkill(111930); // L"Atk3"
				return;
			} else if (length < 55) {// 가까우면 다가가고 아니면 stateFight를 풀어 주자.
				uint32 dir = targetDirection[currentDirection];
				targetPosition = obj->GetPosition();
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				currentDirection = 0;
				if (dir != targetDirection[currentDirection] || GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
					Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
			}
			ChangeState(*stateApproachTarget, updateNormalTick);
		}
		return;
	}

	void StoneGolem::CrazyBegin()
	{
	}

	void StoneGolem::CrazyUpdate()
	{
		float max;
		if (GetHP(max) > max/2.0f) {	//체력이 회복되면 정상모드로
			ChangeState(*stateFightPtr, updateFightTick);
			return;
		}

		if (status.Hp <= 0)	{//웁스.. 죽엇다.
			ChangeState(stateDead, updateNormalTick);
			return;
		}

		if (GetActionNameHash() == Database::Info::HashTable::Actions::Stand ||
			GetActionNameHash() == Database::Info::HashTable::Actions::Walk ||
			GetActionNameHash() == Database::Info::HashTable::Actions::Run) {
			//공격이 풀리고 다가갈수 있게 됐다면, 혹은 다가가는 중이라면

			//공격할 수 있는가.
			std::vector<Player*> l;
			stageData->GetAreaObject(objectData.Position, objectData.Direction, NPCStatus.attackRange, 315, 45, Constants::AllianceType::ExTeam, GetTeam(), this, l);
			for (std::vector<Player*>::iterator i = l.begin() ; i != l.end() ; ++i) {
				if ((*i)->GetSerial() == NPCStatus.target) {//이놈시키 딱걸렸어. 디져바
					if ( !IsAbleToRecognize(*i) ) {//죽었군. 허약한놈
						RemoveHateTarget(NPCStatus.target);
						return;
					}

					if (stageData->GetRandomFloat() >= 85.0f) //매 틱 70%의 확률로 공격 작동.
						return;

					float result = stageData->GetRandomFloat();
					if ( result <= 0.2f)	// 1/3의 확률씩으로 각각의 액션을 취해주자.
						UseSkill(111928); // L"Atk1"
					else if ( result <= 0.4f )
						UseSkill(6484100); // L"Atk2End"
					else if ( result <= 0.6f )
						UseSkill(111930); // L"Atk3"
					else {	// 40%의 확률로 전체 공격
						hitCnt = 0;
						UseSkill(37829995); // L"SkillAtk1"
					}

					return;
				}
			}
			//공격범위에 상대가 없다. 이럴땐.. 가까이 다가가자.
			Actor* obj = stageData->RetrieveActor(NPCStatus.target);
			if (!obj || !IsAbleToRecognize(obj) ) {	//얼씨구.. 이놈 죽었나보다. 다음 대상을 물색하지.
				RemoveHateTarget(NPCStatus.target);
				if (NPCStatus.target == 0)
					//HateList가 비었다. 경계모드로
					ChangeState(stateWarning, updateNormalTick);
				return;
			} else { // 가까우면 다가가고 아니면 stateFight를 풀어 주자.
				// 거리를 잰다.
				float dist = Lunia::Math::Length<float>( float2(objectData.Position.x - obj->GetPosition().x, objectData.Position.z - obj->GetPosition().z) );
				if (dist >= 55) {
					ChangeState(*stateApproachTarget, updateNormalTick);
					return;
				}

				uint32 dir = targetDirection[currentDirection];
				targetPosition = obj->GetPosition();
				AI::DecideDirection(objectData.Position, targetPosition, targetDirection);
				currentDirection = 0;
				if (dir != targetDirection[currentDirection] || GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
					Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
				return;
			}
		}
	}

	void StoneGolem::CrazyEnd()
	{
	}

	void StoneGolem::DeadBegin()
	{
	}

	void StoneGolem::DeadUpdate()
	{
	}

	void StoneGolem::DeadEnd()
	{
	}

	void StoneGolem::EndOperation()
	{
		actions.SetAction(Database::Info::HashTable::Actions::Down, this, NULL, *stageData, true);
	}

}	}	}

