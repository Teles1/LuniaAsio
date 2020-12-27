#include <Core/Utils/StringUtil.h>
#include "../Action/Actions.h"
#include "ActionDB.h"
#include <iostream>
#include <Info/Info.h>

using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated { namespace Logic {

	ActionDB::ActionDB() : info(DatabaseInstance().InfoCollections.Actions)
	{
	}

	ActionDB::~ActionDB()
	{
		
	}
	ActionDB& ActionDB::operator= (const ActionDB& /*db*/)
	{
		info = DatabaseInstance().InfoCollections.Actions;
		return *this;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Initialize Functions
	void ActionDB::Init()
	{
		Info::ActionInfoManager& info = DatabaseInstance().InfoCollections.Actions;
		std::vector<std::wstring> l;
		info.RetrieveTemplateName(l); 
		for ( auto& i : l) {
			CreateActionTemplate( i );
		}
	}

	void ActionDB::CreateActionTemplate(const std::wstring& templateName)
	{
		Info::ActionInfoManager::Actions& actions = info.Retrieve( templateName.c_str() );
		ActionTemplate& actionTemplate = actionMap[ Lunia::StringUtil::Hash(templateName.c_str()) ]; 

		Info::ActionInfoManager::Actions::iterator i = actions.begin(), end = actions.end();
		for (; i != end ; ++i) {
			actionTemplate.AddAction( *i );
		}

		Info::ActionInfoManager::Actions::ActorList::iterator iActor = actions.actorList.begin();
		for (; iActor != actions.actorList.end() ; ++iActor) {
			actionTemplate.AddActor( (*iActor).first, (*iActor).second );
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Retrieve Functions
	Action::Action::ActionPtrList* ActionDB::GetActionList(uint32 templateCode, uint32 objCode)
	{
		std::map<uint32, ActionTemplate>::iterator i = actionMap.find(templateCode);
		if ( i == actionMap.end() ) {
			Logger::GetInstance().Exception( L"[ActionDB::GetActionList] unable to find action list.[{0}/{1}]", templateCode, objCode);
		}
		return (*i).second.GetActionList(objCode);
	}

	//For single debugging
	Action::Action::ActionPtrList* ActionDB::GetActionList(const std::wstring& templateName, const std::wstring obj)
	{
		uint32 templateCode = StringUtil::Hash(templateName.c_str());
		uint32 objCode = StringUtil::Hash(obj.c_str());
#ifdef _SINGLE
		{
			std::map<uint32, ActionTemplate>::iterator i = actionMap.find(templateCode);
			if ( i == actionMap.end() ) {
				CreateActionTemplate(templateName);
			}
			/*
			Action::Action::ActionPtrList* returnValue = (*i).second.GetActionList(objCode);
			if (!returnValue) {			
			}
			*/
		}
			


		
#endif
		auto i = actionMap.find(templateCode);
		if ( i == actionMap.end() )
			throw Exception(( L"[ActionDB::GetActionList] unable to find templateName [%s]", templateName.c_str() ));


		Action::Action::ActionPtrList* returnValue = (*i).second.GetActionList(objCode);
		if (!returnValue) {
			throw Exception( (L"[ActionDB::GetActionList] unable to find objectActions [%s] in template [%s]", obj.c_str(), templateName.c_str()));
		}
		return returnValue;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Action Template functions

	ActionDB::ActionTemplate::ActionTemplate()
	{
	}

	ActionDB::ActionTemplate::~ActionTemplate()
	{
		Action::Action::ActionList::iterator i = allActions.begin();
		for ( ; i != allActions.end() ; ++i ) {
			delete (*i).action;
		}
	}

	ActionDB::ActionTemplate::ActionTemplate(const ActionTemplate&)
	{
	}

	Action::Action::ActionPtrList* ActionDB::ActionTemplate::GetActionList(uint32 hashCode)
	{
		for (auto& i : objects) {
			if (i.hashCode == hashCode)
				return &(i.actions);
		}
		return NULL;
	}

	Action::Action::ActionItem* ActionDB::ActionTemplate::GetAction(uint32 hashCode)
	{
		for (auto& i : allActions) {
			if ( i.hashCode == hashCode )
				return &i;
		}
		return NULL;
	}

	Action::Action::ActionItem* ActionDB::ActionTemplate::GetAction(const std::wstring& name)
	{
		Action::Action::ActionItem* returnValue = GetAction( Lunia::StringUtil::Hash( name.c_str() ) );
		return returnValue;
	}

	void ActionDB::ActionTemplate::AddActor(const std::wstring& objectName, std::vector<std::wstring>& actionList)
	{
		objects.push_back( ObjectActions() );
		ObjectActions& objActions = objects[objects.size()-1];
		objActions.objectName = objectName;
		objActions.hashCode = Lunia::StringUtil::Hash( objectName.c_str() );

		std::vector<std::wstring>::iterator i = actionList.begin();
		for (auto& i : actionList) {
			Action::Action::ActionItem* item = GetAction( i.c_str() );
			if ( !item ) {
				//Lunia_INFO(( L"[ActionDB::ActionTemplate::AddActor] unable to find action [%s / %s]", objectName.c_str(), (*i).c_str() ));
			} else {
				objActions.actions.push_back( item );
			}
		}
	}

	void ActionDB::ActionTemplate::AddAction(Info::ActionInfo* actionInfo)
	{
		Action::Action* action;
		switch (actionInfo->classType) {
		case Info::ActionInfo::ActionClassType::STAND :
			action = new Action::ActionStand(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::MOVE :
			action = new Action::ActionMove(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::DASH :
			action = new Action::ActionDash(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::STUN :
			if ( actionInfo->actionName == L"HitDown")
				action = new Action::ActionBase(actionInfo);
			else
				action = new Action::ActionStun(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::ATTACK :
			action = new Action::ActionAttack(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::DOWNSTART :
			action = new Action::ActionDownStart(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::DOWNAIR :
			action = new Action::ActionDownAir(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::DOWNEND :
			action = new Action::ActionDownEnd(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::DOWNFINISH :
			action = new Action::ActionDownFinish(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::PHOENIXDOWNAIR :
			action = new Action::ActionPhoenixDownAir(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::PHOENIXDOWN2AIR :
			//action = new Action::ActionPhoenixDown2Air(actionInfo); // @ORACLE 01JAN2020 13:29 EST
			break;
		case Info::ActionInfo::ActionClassType::HITAIR :
			action = new Action::ActionHitAir(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::FAINT :
			action = new Action::ActionFaint(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::GHOSTRISE :
			action = new Action::ActionGhostRise(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::SLEEP :
			action = new Action::ActionSleep(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::STRAFE :
			action = new Action::ActionTurn(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::MOVINGKNOCKBACK :
			action = new Action::ActionStepBack(actionInfo);
			break;
		case Info::ActionInfo::ActionClassType::HEIGHTZERO :
			action = new Action::ActionHeightZero(actionInfo);
			break;
		default :
			action = new Action::ActionBase(actionInfo);
		};

		allActions.push_back( Action::Action::ActionItem( Lunia::StringUtil::Hash(actionInfo->actionName.c_str()), actionInfo->actionName, action) );
	}

}	}	}
