#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Action {

		class ActionStun : public Action {
		public :
			struct Param : public Action::IActionParam {
				float stunLength;
				float3 knockBack;

				Param() : stunLength(0), knockBack(float3(0, 0, 0)) {}
				Param(float len, float3 kb) : stunLength(len), knockBack(kb) {}
				Param& operator = (Param& param) {
					this->stunLength = param.stunLength;
					this->knockBack = param.knockBack;
					return *this;
				}
				Action::IActionParam* GetClonePtr() { return new Param(stunLength, knockBack); }
			};

		public :
			ActionStun(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			//virtual ~ActionStun();

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
			void Destroy(Actor* obj, ActionData& data);
		};

}	}	}	}
