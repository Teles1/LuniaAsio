#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionHeightZero : public Action {
		public :
			ActionHeightZero(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionHeightZero() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
		};

	}
}	}	}
