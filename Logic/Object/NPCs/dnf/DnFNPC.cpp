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
	 @ Intialize ������
	 @ �ʱ� NPC���¸� ������ �ش�.
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
		// ȣ��� ���¸� �̹� �þ߿� ����
		
		// 1. ���ٰ���, ������ �Ǵ�.
		
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
				Command( Action::Action::CommandType::STOP, 0); // �켱 ����
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
		
			if (bCollide) {//�浹 �����ΰ�.
				++collisionCnt;
				//��� �̰� 6������ �� AI������Ʈ ƽ���� ����Ǵ°� ���� �����̴�.
				//4�踦 �ؼ� 4ƽ���� �ѹ��� ������ �ٲ�� �ϱ� ���� ������ ��.
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
		
		//������ �� �ִ°�.
		float distance;
		Actor* obj = stageData->RetrieveActor(NPCStatus.target);
		if ( !obj || !IsAbleToRecognize(obj) ||//������ �����ų� �׾��ų�, �þ߸� �����. hatelist ���� ���ش�.
			(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {

			RemoveHateTarget(NPCStatus.target);
			initialPos = objectData.Position;
			ChangeState(*stateIdle, updateNormalTick);

			return;
		} else if (distance <= NPCStatus.attackRange) {//�̳��Ű ���ɷȾ�. ������
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
			// stateFight���� �Ѵ� ġ��, ���⼭ ������
			if ( !obj || !IsAbleToRecognize(obj) ||//������ �����ų� �׾��ų�, �þ߸� �����. hatelist ���� ���ش�.
				(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight) {

				RemoveHateTarget(NPCStatus.target);
				initialPos = objectData.Position;
				ChangeState(*stateIdle, updateNormalTick);

				return;

			}else{

				// ����
				//AI::DecideDirection( obj->GetPosition(), objectData.Position, targetDirection);
				//Command( Action::Action::CommandType::MOVE, targetDirection[0] );
				gostoptime = 0;

				return;
			}
			
		}else{

			//������ �� �ִ°�.
			if ( !obj || !IsAbleToRecognize(obj) ||//������ �����ų� �׾��ų�, �þ߸� �����. hatelist ���� ���ش�.
				(distance = Lunia::Math::Length<float>(obj->GetPosition() - objectData.Position)) > NPCStatus.sight ) {

				RemoveHateTarget(NPCStatus.target);
				initialPos = objectData.Position;
				ChangeState(*stateIdle, updateNormalTick);

				return;
			} else if (distance <= NPCStatus.attackRange) {//�̳��Ű ���ɷȾ�. ������
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