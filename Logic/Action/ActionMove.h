#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Action {

		class ActionMove : public Action {
		public :
			struct Param : public Action::IActionParam {
				float3 direction;

				Param() {}
				Param(const float3& d) : direction(d) {}
				inline Param& operator = (Param& param) {
					direction = param.direction;
					return *this;
				}
				Action::IActionParam* GetClonePtr() { return new Param(direction); }
			};

		public :
			inline ActionMove(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			uint32 GetNextAction(Actor*, IActionParam*&, bool bHit);
		};

		class ActionDash : public Action {
		public :
			inline ActionDash(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
		};

}	}	}	}
