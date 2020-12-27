#include <Info/Info.h>
#include "ActionDown.h"
#include "../Object/Actor.h"
#include "../GameState.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {

		//const float ActionDownStart::SPEED = 100.0f;

		//ActionDownStart	////////////////////////////////////////////////////////////////////////////////////
		void ActionDownStart::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionDownStart::Param* thisParam;
			if ( param )
			{
				thisParam = static_cast<ActionDownStart::Param*>(param);
				if (thisParam==NULL) 
					throw Exception("failed to initilzie ActionDownStart");
			}
			else
			{
				Logger::GetInstance().Exception (L"[ActionDownStart::Initialize] ActionDownStart action must get parameter from caller.");
			}
				

			obj->SetDirection(thisParam->direction);

			if (db->IsAbletoLocate(obj->GetSerial(), obj->GetPosition() + thisParam->knockBack, obj->GetRadius(), obj))
				obj->SetPosition(obj->GetPosition() + thisParam->knockBack);

			Actor::HeightControl& info = obj->GetHeightInfo();
			info.maxHeight = thisParam->speed * thisParam->speed / (240.0f);
			info.maxTime = thisParam->speed / (120.0f);
			info.currentTime = 0;
			info.height = 0;
			info.speed = thisParam->speed;

			Action::Initialize(obj, db, param, data);	//should be called.
		}

		bool ActionDownStart::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			Action::Do(dt, obj, db, data);
			float3 pos = obj->GetPosition();
			Actor::HeightControl& info= obj->GetHeightInfo();
			info.currentTime += dt;

			if(info.maxTime < 0.001f)
			{
				info.height = 0.0f;
				return true;
			}
			float h;
			if (info.currentTime < info.maxTime) {
				h = ( -pow(-info.currentTime/info.maxTime+1, 2.5f) + 1) * info.maxHeight;
				obj->SetHeight(pos.y + h-info.height);
				info.height = h;
				return false;
			} else {
				h = ( -pow(info.currentTime/info.maxTime-1, 2.5f) + 1) * info.maxHeight;
				obj->SetHeight(pos.y + h-info.height);
				info.height = h;
			}
			return true;
		}


		// ActionDownAir	////////////////////////////////////////////////////////////////////////////////////
		void ActionDownAir::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			Actor::HeightControl& info= obj->GetHeightInfo();
			if ( info.speed > 0 )
				info.speed = 0 ;

			Action::Initialize(obj, db, param, data);	//should be called.
		}

		bool ActionDownAir::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			Action::Do(dt, obj, db, data);
			float3 pos = obj->GetPosition();
			Actor::HeightControl& info= obj->GetHeightInfo();
			info.currentTime += dt;
			float ch=0, h;
			if ( info.speed < 0 ) {
				h = pos.y + info.speed * dt;
			} else {

				if (info.maxTime == 0)
				{
					info.height = 0.0f;
					obj->SetHeight(0);
					return true;
				}
				else if (info.currentTime < info.maxTime) {
					ch = ( -pow(-info.currentTime/info.maxTime+1, 2.5f) + 1) * info.maxHeight;
				} else {
					ch = ( -pow(info.currentTime/info.maxTime-1, 2.5f) + 1) * info.maxHeight;
				}
				h = pos.y - (info.height-ch);
			}
			if (h<=0) {
				obj->SetHeight(0);
				return true;
			} else
	            obj->SetHeight(h);
			info.height = ch;
			return false;
		}


		// ActionDownEnd	////////////////////////////////////////////////////////////////////////////////
		void ActionDownEnd::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			obj->GetHeightInfo().speed = 0;
			Action::Initialize(obj, db, param, data);	//should be called.
		}

		const Database::Info::ActionInfo* ActionDownEnd::GetLinkedAction(Actor* actor, unsigned int key, float dt, bool &reserved, float& changeTime, bool bHit)
		{
			float max;
			if (actor->GetType() == Constants::ObjectType::Player && actor->GetHP(max) <= 0)
				return actor->GetActionInfo( Database::Info::HashTable::Actions::GhostRise );
			return Action::GetLinkedAction(actor, key, dt, reserved, changeTime, bHit);
		}

		uint32 ActionDownEnd::GetNextAction(Actor* actor, IActionParam*& param, bool bHit)
		{
			float max;
			if (actor->GetType() == Constants::ObjectType::Player && actor->GetHP(max) <= 0)
				return Database::Info::HashTable::Actions::GhostRise;
			return Action::GetNextAction(actor, param, bHit);
		}


		// ActionDownFinish	////////////////////////////////////////////////////////////////////////////////
		void ActionDownFinish::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionDownFinish::Param* thisParam;
			if ( param )
				thisParam = (ActionDownFinish::Param*)param;
			else {
				thisParam = new ActionDownFinish::Param();
				data.actionSpecificParams = thisParam;
			}
			Action::Initialize(obj, db, param, data);	//should be called.
		}

		bool ActionDownFinish::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			bool returnValue = Action::Do(dt, obj, db, data);	//should be called.
			if(returnValue)
				return returnValue;

			ActionDownFinish::Param* param = (ActionDownFinish::Param*)(data.actionSpecificParams);

			param->riseTick += dt;
			if (param->riseTick >= 0.1f) {
				param->riseTick -= 0.1f;
				if (db->GetRandomFloat() < 0.125f)
					return true;
			}
			return false;
		}

		void ActionDownFinish::Destroy(Actor* obj, ActionData& data)
		{
			return Action::Destroy(obj, data);
		}

		// ActionHitAir	////////////////////////////////////////////////////////////////////////////////////
		void ActionHitAir::Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data)
		{
			ActionHitAir::Param* thisParam;
			if ( param )
				thisParam = (ActionHitAir::Param*)param;
			else {
				Logger::GetInstance().Exception( L"[ActionHitAir::Initialize] ActionHitAir action must get parameter from caller.");
			}

			Actor::HeightControl& info = obj->GetHeightInfo();
			info.speed = thisParam->speed;
			if ( info.speed >= 0 ) {
				info.maxHeight = pow(info.speed, 2) / (240.0f);
				info.maxTime = info.speed / (120.0f);
			} else {
				info.maxHeight = info.height;
				if (info.speed )
					info.maxTime = - info.maxHeight / info.speed;
				else
					info.maxTime = 0;
			}
			info.currentTime=0;
			info.height = 0;
			////////////////////////////////////////////


			float3 position = obj->GetPosition();
			position.y = 0;
			if (db->IsAbletoLocate(obj->GetSerial(), position + thisParam->knockBack, obj->GetRadius(), obj))
				obj->SetPosition(obj->GetPosition() + thisParam->knockBack);
			////////////////////////////////////////////

			Action::Initialize(obj, db, param, data);	//should be called.
		}

		bool ActionHitAir::Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data)
		{
			Action::Do(dt, obj, db, data);

			float3 pos = obj->GetPosition();
			Actor::HeightControl& info= obj->GetHeightInfo();
			info.currentTime += dt;
			float h;

			if ( info.speed >= 0 ) {
				if (info.maxTime == 0) 
					return true;
				else if (info.currentTime < info.maxTime) {
					h = ( -pow(-info.currentTime/info.maxTime+1, 2.5f) + 1) * info.maxHeight;
					obj->SetHeight(pos.y + h-info.height);
					info.height = h;
					return false;
				} else {
					h = ( -pow(info.currentTime/info.maxTime-1, 2.5f) + 1) * info.maxHeight;
					obj->SetHeight(pos.y + h-info.height);
					info.height = h;
				}
			} else {
				float height = pos.y + info.speed * dt;
				if ( height <= 0 ) {
					obj->SetHeight(0);
					return true;
				} else {
					obj->SetHeight(height);
					return false;
				}

			}
			
			return true;
		}
	}
}	}	}
