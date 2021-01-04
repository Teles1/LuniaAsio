#pragma once

#include "Action.h"

namespace Lunia {
	namespace XRated {
		namespace Logic {
			namespace Action {
				class ActionHitAllPlayer : public Action {
				public:
					ActionHitAllPlayer(Database::Info::ActionInfo* i) : Action(i) {}
					virtual ~ActionHitAllPlayer() {}

					void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
				};

			}
		}
	}
}
