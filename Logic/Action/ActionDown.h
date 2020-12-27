#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {

		class ActionDownStart : public Action {
		public :
			//const static float SPEED;

			struct Param : public Action::IActionParam {
			public :
				float speed;
				float3 direction;
				float3 knockBack;

				Param() : speed(0), direction(float3(0, 0, 0)), knockBack(float3(0, 0, 0)) { }
				Param(float s, float3 dir, float3 kb) : speed(s), direction(dir), knockBack(kb) {}
				Param& operator = (const Param& param) {
					speed = param.speed;
					direction = param.direction;
					knockBack = param.knockBack;
					return *this;
				}

				Action::IActionParam* GetClonePtr() { return new Param(speed, direction, knockBack); }
			};

		public :
			ActionDownStart(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionDownStart() {}
			ActionDownStart& operator= (const ActionDownStart& /*action*/)
			{
				Logger::GetInstance().Exception( "[ActionDownStart::operator=] action class cannot have assignment function.");
			}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
		};

		class ActionDownAir : public Action {
		public :
			ActionDownAir(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionDownAir() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
		};

		class ActionDownEnd : public Action {
		public :
			ActionDownEnd(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionDownEnd() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			const Database::Info::ActionInfo* GetLinkedAction(Actor* actor, unsigned int key, float dt, bool &reserved, float& changeTime, bool bHit);
			uint32 GetNextAction(Actor* actor, IActionParam*& param, bool bHit);
		};

		class ActionDownFinish : public Action {
			struct Param : public Action::IActionParam {
				float riseTick;
				Param () : riseTick(0) {}

				Action::IActionParam* GetClonePtr() { return new Param; }
			};

		public :
			ActionDownFinish(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionDownFinish() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
			void Destroy(Actor* obj, ActionData& data);
		};

		class ActionHitAir : public Action {
		public :
			struct Param : public Action::IActionParam {
				float speed;
				float3 direction;
				float3 knockBack;

				float currentHeight;

				Param() : speed(0), direction(float3(0, 0, 0)), knockBack(float3(0, 0, 0)), currentHeight(0) { }
				Param(float s, float3 dir, float3 kb) : speed(s), direction(dir), knockBack(kb), currentHeight(0) {}
				Param& operator = (const Param& param) {
					speed = param.speed;
					direction = param.direction;
					knockBack = param.knockBack;
					currentHeight = param.currentHeight;
					return *this;
				}

				Action::IActionParam* GetClonePtr() { return new Param(speed, direction, knockBack); }
			};

		public :
			ActionHitAir(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionHitAir() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
		};

	}
}	}	}
