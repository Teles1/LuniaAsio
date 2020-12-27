#include <Info/Info.h>
#include "dnfnpc.h"
#include "../../Player.h"
#include "../../../GameState.h"
#include "../../../AI/AIGlobalFunctions.h"

using namespace Lunia::Math;
using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Logic::AI;

namespace Lunia { namespace XRated {	namespace Logic {

	DnFNPC::DnFNPC(void)
	{
	}

	DnFNPC::~DnFNPC(void)
	{
	}

	/**
	 @ Intialize 재정의
	 @ 초기 NPC형태를 정의해 준다.
	*/
	void DnFNPC::Initialize(Lunia::Math::RandomInt& rInt)
	{
		NonPlayer::Initialize(rInt);
		
		gostoptime = 0;
		isgo = false;

		stateApproachTarget = &stateApproachTarget_Sight;
		stateCaution.Set( L"caution", this, &NonPlayer::stateCaution_SightBegin, &NonPlayer::stateCaution_SightUpdate, &NonPlayer::stateCaution_SightEnd );
	}
	
	//void DnFNPC::IdleDefenceBegin()
	//{
	//}
	//void DnFNPC::IdleDefenceUpdate()
	//{
	//}
	//void DnFNPC::IdleDefenceEnd()
	//{
	//}

	
	void DnFNPC::stateApproachTarget_SightBegin()
	{
		// 호출된 상태면 이미 시야에 들어옴
		
		// 1. 가다갈지, 멈출지 판단.
		
		int rnd = static_cast< int >( stageData->GetRandomInt( 0, 3 ) );
		if( rnd == 1 )
		{
			// true
			isgo = true;	
		}else{
			// false
			isgo = false;
		}

	}

	void DnFNPC::stateApproachTarget_SightUpdate()
	{
		gostoptime += 0.5f;

		if( gostoptime >= 3.0f )
		{
			int rnd = static_cast< int >( stageData->GetRandomInt( 0, 2 ) );
			if( rnd == 1 )
			{
				isgo = !isgo;
				Command( Action::Action::CommandType::STOP, 0); // 우선 멈춤
			}

			gostoptime = 0;
		}

		if( isgo == true )
		{
			uint32 dir = targetDirection[currentDirection];
			Actor* obj = stageData->RetrieveActor(NPCStatus.target);
			if ( !obj || !IsAbleToRecognize(obj) )
			{
				ChangeState( *stateIdle, updateNormalTick );
				return;
			}

			AI::DecideDirection(objectData.Position, obj->GetPosition(), targetDirection);
		
			if (bCollide) {//충돌 상태인가.
				++collisionCnt;
				//사실 이건 6방향을 매 AI업데이트 틱마다 변경되는걸 막기 위함이다.
				//4배를 해서 4틱마다 한번씩 방향이 바뀌게 하기 위한 뻘수지 뭐.
				if (collisionCnt == 24)
					collisionCnt = 0;
				currentDirection = collisionCnt/4;
				Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
			} else {
				currentDirection = collisionCnt = 0;
				if (dir != targetDirection[currentDirection] || GetActionNameHash() == Database::Info::HashTable::Actions::Stand)
					Command(Action::Action::CommandType::MOVE, targetDirection[currentDirection]);
			}

		}
		
		//공격할 수 있는가.
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//게임을 나갔거나 죽었거나, 시야를 벗어났다. hatelist 에서 빼준다.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {

			RemoveHateTarget(NPCStatus.target);
			initialPos = objectData.Position;
			ChangeState(*stateIdle, updateNormalTick);

			return;
		} else if (distance <= NPCStatus.attackRange) {//이놈시키 딱걸렸어. 디져바
			ChangeState(*stateFightPtr, updateFightTick);
			return;
		}

	}

	void DnFNPC::stateApproachTarget_SightEnd()
	{
	}

	void DnFNPC::stateCaution_SightBegin()
	{
		//gostoptime = 0;

		int rnd = static_cast< int >( stageData->GetRandomInt( 0, 3 ) );

		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);

		if( rnd == 0 )
		{
			// stateFight에서 한대 치고, 여기서 빠지고
			if ( !obj || !IsAbleToRecognize(obj) ||//게임을 나갔거나 죽었거나, 시야를 벗어났다. hatelist 에서 빼준다.
				(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {

				RemoveHateTarget(NPCStatus.target);
				initialPos = objectData.Position;
				ChangeState(*stateIdle, updateNormalTick);

				return;

			}else{

				// 도망
				//AI::DecideDirection( obj->GetPosition(), objectData.Position, targetDirection);
				//Command( Action::Action::CommandType::MOVE, targetDirection[0] );
				gostoptime = 0;

				return;
			}
			
		}else{

			//공격할 수 있는가.
			if ( !obj || !IsAbleToRecognize(obj) ||//게임을 나갔거나 죽었거나, 시야를 벗어났다. hatelist 에서 빼준다.
				(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight ) {

				RemoveHateTarget(NPCStatus.target);
				initialPos = objectData.Position;
				ChangeState(*stateIdle, updateNormalTick);

				return;
			} else if (distance <= NPCStatus.attackRange) {//이놈시키 딱걸렸어. 디져바
				ChangeState(*stateFightPtr, updateFightTick);
				return;
			} else
				ChangeState( *stateApproachTarget, updateNormalTick );
		}
	}

	void DnFNPC::stateCaution_SightUpdate()
	{
		if( GetActionName() == L"Stand" )
		{
			Actor* obj = stageData->RetrieveActor(NPCStatus.target);
			if ( !obj || !IsAbleToRecognize(obj) )
			{
				ChangeState( *stateIdle, updateNormalTick );
				return;
			}

			AI::DecideDirection( obj->GetPosition(), objectData.Position, targetDirection);
			Command( Action::Action::CommandType::MOVE, targetDirection[0] );
			gostoptime = 0.5f;
		}

		if( GetCurrentAction()->GetActionClassType() == Database::Info::ActionInfo::ActionClassType::MOVE )
		{
			gostoptime += 0.5f;
		}

		if( gostoptime >= 2.5f )
		{	
			
			gostoptime = 0;
			ChangeState( *stateApproachTarget, updateNormalTick );
		}
	}

	void DnFNPC::stateCaution_SightEnd()
	{
		Command( Action::Action::CommandType::STOP, 0);
	}

	void DnFNPC::FightBegin()
	{
		NonPlayer::FightBegin();
		/*
		if (GetActionName() != L"Stand")
			Command(Action::Action::CommandType::STOP, L"");
		*/
	}

} } }