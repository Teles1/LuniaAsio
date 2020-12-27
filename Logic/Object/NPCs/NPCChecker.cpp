#include "NPCChecker.h"
#include <Info/Info/Database.h>
#include <Logic/Action/Actions.h>
#include <iostream>

namespace Lunia { namespace XRated {	namespace Logic {

	void NPCChecker::SetAction( uint32 name )
	{
		const Database::Info::ActionInfo* info = GetActionInfo( name );

		if ( !info ) {
			Logger::GetInstance().Exception(L"[NPCChecker::SetAction] unable to find action.({0}/{1})", GetTemplateName().c_str(), name);
		}
		switch ( info->classType ) {
		case Database::Info::ActionInfo::ActionClassType::ATTACK :
			actions.SetAction(name, this, NULL, *stageData, true);
			break;
		case Database::Info::ActionInfo::ActionClassType::DASH :
			actions.SetAction(name, this, NULL, *stageData, true);
			break;
		case Database::Info::ActionInfo::ActionClassType::DOWNAIR :
			actions.SetAction(name, this, NULL, *stageData, true);
			break;
		case Database::Info::ActionInfo::ActionClassType::DOWNEND :
			actions.SetAction(name, this, NULL, *stageData, true);
			break;
		case Database::Info::ActionInfo::ActionClassType::DOWNFINISH :
			actions.SetAction(name, this, NULL, *stageData, true);
			break;
		case Database::Info::ActionInfo::ActionClassType::DOWNSTART :
			{
				Action::ActionDownStart::Param param(100, Constants::DirectionF::Down, Constants::DirectionF::Down);
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		case Database::Info::ActionInfo::ActionClassType::FAINT :
			actions.SetAction(name, this, NULL, *stageData, true);
			break;
		case Database::Info::ActionInfo::ActionClassType::GHOSTRISE :
			Logger::GetInstance().Exception(L"[NPCChecker::SetAction] Nonplayer actions cannot have GHOSTRISE class type.");
			break;
		case Database::Info::ActionInfo::ActionClassType::HITAIR :
			{
				Action::ActionHitAir::Param param(50, Constants::DirectionF::Down, Constants::DirectionF::Down);
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		case Database::Info::ActionInfo::ActionClassType::MOVE :
			{
				SetBufferedDirection(Constants::DirectionF::Down);
				Action::ActionMove::Param param(Constants::DirectionF::Down);
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		case Database::Info::ActionInfo::ActionClassType::NORMAL :
			actions.SetAction(name, this, NULL, *stageData, true);
			break;
		case Database::Info::ActionInfo::ActionClassType::PHOENIXDOWNAIR :
			{
				Action::ActionPhoenixDownAir::Param param(0.5f, Constants::DirectionF::Down);
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		case Database::Info::ActionInfo::ActionClassType::PHOENIXDOWN2AIR :
			{
				Action::ActionPhoenixDown2Air::Param param(1.0f, Constants::DirectionF::Down); // @oracle 01JAN2020 13:30 EST
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		/*case Database::Info::ActionInfo::ActionClassType::CENTERMOVE : //Needs added @oracle 01JAN2020 19:44 EST
			{
				Action::ActionCenterMove::Param param(1.0f, Constants::DirectionF::Down);
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;*/
		case Database::Info::ActionInfo::ActionClassType::SLEEP :
			{
				Action::ActionSleep::Param param(2, 100, Action::ActionSleep::Direction::LEFT );
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		case Database::Info::ActionInfo::ActionClassType::STAND :
			Logger::GetInstance().Exception(L"[NPCChecker::SetAction] Nonplayer actions cannot have STAND class type." );
			break;
		case Database::Info::ActionInfo::ActionClassType::STRAFE :
			Logger::GetInstance().Exception(L"[NPCChecker::SetAction] Nonplayer actions cannot have STRAFE class type." );
			break;
		case Database::Info::ActionInfo::ActionClassType::STUN :
			{
				Action::ActionStun::Param param(1, Constants::DirectionF::Down);
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		case Database::Info::ActionInfo::ActionClassType::MOVINGKNOCKBACK :
			{
				Action::ActionStepBack::Param param(2, Constants::DirectionF::Down);
				actions.SetAction(name, this, &param, *stageData, true);
			}
			break;
		default :
			actions.SetAction( name, this, NULL, *stageData, true);
			break;
		}

		Logger::GetInstance().Info(L"[NPCChecker::SetAction] [{0}] Action set to {1}", GetName().c_str(), name );
	}

	bool NPCChecker::NextAction()
	{
		const std::vector<std::wstring>* actionList =
			Database::DatabaseInstance().InfoCollections.Actions.RetrieveActionList( NPCStatus.npcInfo->ActionListName.c_str(), GetTemplateName().c_str() );
		if ( actionCnt >= (int)actionList->size() )
			return false;

		SetAction( StringUtil::Hash(actionList->at(actionCnt).c_str()) );
		++actionCnt;

		return true;
	}

	void NPCChecker::IdleNoneBegin()
	{
		const std::vector<std::wstring>* actionList =
			Database::DatabaseInstance().InfoCollections.Actions.RetrieveActionList( NPCStatus.npcInfo->ActionListName.c_str(), GetTemplateName().c_str() );
		SetAction( StringUtil::Hash( actionList->at(0).c_str() ) );
		++actionCnt;
	}

	void NPCChecker::IdleNoneUpdate()
	{
		const Database::Info::ActionInfo* info;
		info = actions.GetAction(L"")->GetActionInfo();

		if ( GetActionName() == L"Stand" ) {
			if ( !NextAction() ) {
				SetHP(0);
				return;
			}
		} else if ( info->playTime == -1 ) { 
			actionPlayTime += 0.5f;
			if ( actionPlayTime >= 3.0f ) {
				if ( !NextAction() ) {
					SetHP(0);
					return;
				}
			}
		}
	}

	void NPCChecker::IdleNoneEnd()
	{
	}

}	}	}

