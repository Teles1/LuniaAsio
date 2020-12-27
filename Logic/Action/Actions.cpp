#include "Actions.h"

#include "../State/StateSleep.h"

#include "../Object/TombStone.h"
#include "../Object/Player.h"
#include "../GameState.h"
#include "../LogicDB/LogicDB.h"
#include <iostream> //@oracle 01JAN2020 14:29 EST

//using namespace Lunia::XRated::Logic;
using namespace Lunia::XRated::Database::Info;

namespace Lunia { namespace XRated { namespace Logic { namespace Action {

		void ActionAttack::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			float3 dir = obj->GetBufferedDirection();
			if (dir.x || dir.z) {
				obj->SetDirection(dir);
			}
			Action::Initialize(obj, db, param, data);	//should be called.
		}

		void ActionDash::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionMove::Param* thisParam;
			if (param)
				thisParam = (ActionMove::Param*)param;
			else {
				Logger::GetInstance().Exception(L"[ActionDash::Initialize] ActionDash action must get parameter from caller.");
			}

			obj->SetDirection( thisParam->direction );

			Action::Initialize(obj, db, param, data);	//should be called.
		}

		void ActionFaint::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionFaint::Param* thisParam;
			if ( param )
				thisParam = (ActionFaint::Param*)param;
			else {
				thisParam = new ActionFaint::Param(playTime);
				data.actionSpecificParams = thisParam;
			}
			Action::Initialize(obj, db, param, data);
		}

		void ActionFaint::ReduceTime(int d, ActionData& data)
		{
			ActionFaint::Param* param = (ActionFaint::Param*)(data.actionSpecificParams);

			if ( (Direction)d != param->dir)
				return;

			switch (param->dir) {
			case Direction::RIGHT :
				param->dir = Direction::LEFT;
				break;
			case Direction::LEFT :
				param->dir = Direction::RIGHT;
			}
			param->playTime = (param->playTime <= 0 ? 0 : param->playTime - 0.1f);
		}

		bool ActionFaint::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			Action::Do(dt, obj, db, data);
			ActionFaint::Param* param = (ActionFaint::Param*)(data.actionSpecificParams);
			if (data.passedTime >= param->playTime) {
				return true;
			}
			return false;
		}

		void ActionFaint::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}

		void ActionGhostRise::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ObjectData& objData = obj->GetObjectData();
			if (objData.Type != Constants::ObjectType::Player) {
				Logger::GetInstance().Exception(L"[ActionGhostRise::Initialize] {0} is not player. ", objData.Name.c_str());
				return;
			}
            ((Player*)obj)->Die();
			((Player*)obj)->SetHeight(0);
			Action::Initialize(obj, db, param, data); 
		}

		void ActionHeightZero::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			obj->GetHeightInfo().Init();
			obj->SetHeight(0);
			obj->GetCharacterData().Speed = 0;

			Action::Initialize(obj, db, param, data);
		}

		void ActionMove::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionMove::Param* thisParam;
			if ( param )
				thisParam = (ActionMove::Param*)param;
			else {
				obj->SetDirection(XRated::Constants::DirectionF::Down);
				//throw( Lunia_EXCEPTION((  )) );
				Logger::GetInstance().Exception(L"[ActionMove::Initialize] ActionMove action must get parameter from caller.[{0}]", obj->GetName().c_str() );
			}
			obj->SetDirection(thisParam->direction );
			Action::Initialize(obj, db, param, data);	//should be called.
		}

		uint32 ActionMove::GetNextAction(Actor* obj, IActionParam*& param, bool bHit)
		{
			ActionMove::Param* p = new ActionMove::Param;
			p->direction = obj->GetBufferedDirection();
			if (param) delete param;
			param = p;
			return Action::GetNextAction(obj, param, bHit);
		}


		void ActionPhoenixDownAir::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionPhoenixDownAir::Param* thisParam;
			if ( param ) {
				thisParam = (ActionPhoenixDownAir::Param*)param;
			} else {
				Logger::GetInstance().Exception(L"[ActionPhoenixDownAir::Initialize] ActionPhoenixDownAir action must get parameter from caller.");
			}

			obj->SetDirection(thisParam->direction);

			Actor::HeightControl& heightInfo = obj->GetHeightInfo();
			heightInfo.speed = 0;
			heightInfo.height = 0;
			heightInfo.maxHeight = 0;
			obj->SetHeight(0);
			Action::Initialize(obj, db, param, data);
		}

		void ActionPhoenixDown2Air::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionPhoenixDown2Air::Param* thisParam;
			if ( param ) {
				thisParam = (ActionPhoenixDown2Air::Param*)param;
			} else {
				Logger::GetInstance().Exception(L"[ActionPhoenixDownAir2::Initialize] ActionPhoenixDown2Air action must get parameter from caller.");
			}

			obj->SetDirection(thisParam->direction);

			Actor::HeightControl& heightInfo = obj->GetHeightInfo();
			heightInfo.speed = 0;
			heightInfo.height = 0;
			heightInfo.maxHeight = 0;
			obj->SetHeight(0);
			Action::Initialize(obj, db, param, data);
		}

		void ActionCenterMove::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionCenterMove::Param* thisParam;
			if ( param ) {
				thisParam = (ActionCenterMove::Param*)param;
			} else {
				Logger::GetInstance().Exception(L"[ActionCenterMove::Initialize] ActionCenterMove action must get parameter from caller.");
			}

			obj->SetDirection(thisParam->direction);

			Actor::HeightControl& heightInfo = obj->GetHeightInfo();
			heightInfo.speed = 0;
			heightInfo.height = 0;
			heightInfo.maxHeight = 0;
			obj->SetHeight(0);
			Action::Initialize(obj, db, param, data);
		}

		bool ActionPhoenixDownAir::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			ActionPhoenixDownAir::Param* param = (ActionPhoenixDownAir::Param*)(data.actionSpecificParams);
			Action::Do(dt, obj, db, data);
			if (obj->IsCollide() || data.passedTime > param->duration)
				return true;
			return false;
		}

		bool ActionPhoenixDown2Air::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			ActionPhoenixDown2Air::Param* param = (ActionPhoenixDown2Air::Param*)(data.actionSpecificParams);
			Action::Do(dt, obj, db, data);
			if (obj->IsCollide() || data.passedTime > param->duration)
				return true;
			return false;
		}

		bool ActionCenterMove::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			ActionCenterMove::Param* param = (ActionCenterMove::Param*)(data.actionSpecificParams);
			Action::Do(dt, obj, db, data);
			if (obj->IsCollide() || data.passedTime > param->duration)
				return true;
			return false;
		}

		void ActionPhoenixDownAir::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}

		void ActionPhoenixDown2Air::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}

		void ActionCenterMove::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}

		bool ActionSleep::IsPossible(const std::wstring &, float, Actor*, ActionData& data)
		{
			ActionSleep::Param* param = (ActionSleep::Param*)(data.actionSpecificParams);
			if (data.passedTime < param->duration)
				return false;
			return true;
		}

		void ActionSleep::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionSleep::Param* thisParam;
			if ( param )  {
				thisParam = (ActionSleep::Param*)param;
			} else {
				Logger::GetInstance().Exception(L"[ActionSleep::Initialize] ActionSleep action must get parameter from caller.");
			}

			Action::Initialize(obj, db, param, data);	//should be called.
		}
		// add by kds218
		void ActionSleep::ReduceTime(int d, ActionData& data)
		{
			ActionSleep::Param* param = (ActionSleep::Param*)(data.actionSpecificParams);			

			if ( (Direction)d != param->direction )
				return;

			switch (param->direction) {
			case Direction::RIGHT :
				param->direction = Direction::LEFT;
				break;
			case Direction::LEFT :
				param->direction = Direction::RIGHT;
			}
			param->duration = (param->duration <= 0 ? 0 : param->duration - 0.1f);
		}
		// edit by kds218
		bool ActionSleep::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			ActionSleep::Param* param = (ActionSleep::Param*)(data.actionSpecificParams);

			Action::Do(dt, obj, db, data);

			if (data.passedTime > param->duration)
			{
				Logger::GetInstance().Warn(L"[ActionSleep::Do] Sleep Duration : {0}", param->duration );
				return true;
			}
			return false;
		}

		void ActionSleep::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}


		bool ActionStand::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			float3 dir = obj->GetBufferedDirection();
			if (dir.x || dir.z)
				return true;
			return Action::Do(dt, obj, db, data);
		}

		uint32 ActionStand::GetNextAction(Actor* obj, IActionParam*& param, bool /*bHit*/)
		{
			ActionMove::Param* p = new ActionMove::Param;
			p->direction = obj->GetBufferedDirection();
			if (param) delete param;
			param = p;
			return Database::Info::HashTable::Actions::Run;
		}

		void ActionStepBack::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionStepBack::Param* thisParam;
			if ( param ) {
				thisParam = (ActionStepBack::Param*)param;
			} else {
				Logger::GetInstance().Exception(L"[ActionStepBack::Initialize] ActionStepBack action must get parameter from caller.");
			}

			obj->SetDirection(thisParam->direction);

			Action::Initialize(obj, db, param, data);
		}

		bool ActionStepBack::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			ActionStepBack::Param* param = (ActionStepBack::Param*)(data.actionSpecificParams);

			Action::Do(dt, obj, db, data);

			if ( data.passedTime > param->duration )
				return true;
			return false;
		}

		void ActionStepBack::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}

		void ActionStun::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionStun::Param* thisParam;
			if ( param ) {
				thisParam = (ActionStun::Param*)param;
			} else {
				Logger::GetInstance().Exception(L"[ActionStun::Initialize] ActionStun action must get parameter from caller.");
			}

			if (!obj->CheckState(StateInfo::Type::ROOT)) {
				if (db->IsAbletoLocate(obj->GetSerial(), obj->GetPosition() + thisParam->knockBack, obj->GetRadius(), obj))
					obj->SetPosition(obj->GetPosition() + thisParam->knockBack);
			}
			Action::Initialize(obj, db, param, data);	//should be called.
		}

		bool ActionStun::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			ActionStun::Param* param = (ActionStun::Param*)(data.actionSpecificParams);
	
			Action::Do(dt, obj, db, data);

			if (data.passedTime > param->stunLength)
				return true;
			return false;
		}

		void ActionStun::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}


		void ActionTurn::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionTurn::Param* thisParam;
			if ( param ) {
				thisParam = (ActionTurn::Param*)param;
			} else {
				Logger::GetInstance().Exception(L"[ActionTurn::Initialize] ActionTurn action must get parameter from caller.");
			}

			Action::Initialize(obj, db, param, data);	//should be called.
		}

		void ActionTurn::Destroy(Actor* obj, ActionData& data)
		{
			ActionTurn::Param* param = (ActionTurn::Param*)(data.actionSpecificParams);
			obj->SetDirection(param->direction);

			return Action::Destroy(obj, data);
		}
}	}	}	}

