#pragma once

#include "../Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionTurn : public Action {
			struct Param : public Action::IActionParam {
				float3 direction;

				Param() : direction(0, 0, 0) {}
				Param(float3 dir) : direction(dir) {}
				Param& operator = (Param& param) {
					direction = param.direction;
					return *this;
				}
				Action::IActionParam* GetClonePtr() { return new Param(direction); }
			};

		public :
			ActionTurn(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionTurn() {}

			virtual void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			virtual void Destroy(Actor* obj, ActionData& data);
		};

	}
}	}	}
