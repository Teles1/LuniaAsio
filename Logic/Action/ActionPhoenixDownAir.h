#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionPhoenixDownAir : public Action {
		public :
			struct Param : public Action::IActionParam {
				float duration;
				float3 direction;

				Param() : duration(0), direction(0, 0, -1) {}
				Param(float dur, float3 dir) : duration(dur), direction(dir) {}
				Param& operator = (const Param& param) {
					duration = param.duration;
					direction = param.direction;
					return *this;
				}
				Action::IActionParam* GetClonePtr() { return new Param(duration, direction); }
			};
		public :
			ActionPhoenixDownAir(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionPhoenixDownAir() {}

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
			void Destroy(Actor* obj, ActionData& data);
		};
	}
}	}	}
