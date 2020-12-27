#pragma once

#include "Action.h"

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {
		class ActionSleep : public Action {
		public :

			enum Direction { LEFT = 0, RIGHT = 1};

			struct Param : public Action::IActionParam {
				Direction direction;
				float duration;
				float buffRate;

				Param() : duration(0), buffRate(0), direction(Direction::LEFT) {}
				Param(float dur, float buff, Direction dir) : duration(dur), buffRate(buff), direction(dir) {}
				Param& operator = (const Param& param) {
					this->duration = param.duration;
					this->buffRate = param.buffRate;
					this->direction = param.direction;
					return *this;
				}
				Action::IActionParam* GetClonePtr() { return new Param(duration, buffRate, direction); }
			};

		public :
			ActionSleep(Lunia::XRated::Database::Info::ActionInfo* i) : Action(i) {}
			virtual ~ActionSleep() {}

			bool IsPossible(const std::wstring &name, float, Actor*, ActionData&);
			void Initialize(Actor* obj, IGameStateDatabase* db, IActionParam* param, ActionData& data);
			void ReduceTime(int d, ActionData& data);
			bool Do(float dt, Actor* obj, IGameStateDatabase* db, ActionData& data);
			void Destroy(Actor* obj, ActionData& data);
			void Awaken(ActionData& data) { ((Param*)(data.actionSpecificParams))->duration = data.passedTime; }
		};

	}
}	}	}
