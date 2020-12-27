#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionGhostRise : public Action {
		public :
			ActionGhostRise(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionGhostRise() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
		};

	}
}	}	}
