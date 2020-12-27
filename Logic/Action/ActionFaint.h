#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionFaint : public Action {
		public :
			enum Direction { LEFT = 0, RIGHT = 1};

			struct Param : public Action::IActionParam {
				Direction dir;
				float playTime;
				Param(float t) : dir(Direction::LEFT), playTime(t) {}

				Action::IActionParam* GetClonePtr() { return new Param(playTime); }
			};

		protected :
			float playTime;

		public :
			ActionFaint(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i), playTime(i->playTime) {}
			//virtual ~ActionFaint();

			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
			void Destroy(Actor* obj, ActionData& data);
			void ReduceTime(int d, ActionData& data);
		};

	}
}	}	}
