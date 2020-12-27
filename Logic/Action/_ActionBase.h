#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionBase : public Action {
		public :
			ActionBase(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionBase() {}

			virtual void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data) {
				Action::Initialize(obj, db, param, data);	}//should be called.
			virtual bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data) {
				return Action::Do(dt, obj, db, data); }
			virtual void Destroy(Actor* obj, ActionData& data) { return Action::Destroy(obj, data); }
		};

	}
}	}	}
