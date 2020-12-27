#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionAttack : public Action {

		public :
			ActionAttack(Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionAttack() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
		};

	}
}	}	}
