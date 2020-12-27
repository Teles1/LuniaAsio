#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionStand : public Action {
		public :
			ActionStand(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionStand() {}

			uint32 GetNextAction(Actor* obj, IActionParam*& param, bool bHit);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
		};

	}
}	}	}
