#pragma once

#include "State.h"


namespace Lunia {
	namespace XRated {
		namespace Logic {
			namespace Stat {

				class EnableAddDefense : public State<Actor> {
				public:
					enum ChargeType {
						RATE, AMOUNT
					};
					enum EnableState {
						NEEDINIT = 0, ENABLE, DISABLE,
					};
				protected:
					ChargeType chargeType;
					float amount;
					int stateEnable;

				public:
					EnableAddDefense(Actor* owner, ChargeType ct, float a)
						: State<Actor>(owner, Database::Info::StateInfo::Type::ENABLEADDDEFENCE, Database::Info::StateInfo::Sort::DEFENCESTATE),
						chargeType(ct), amount(a), stateEnable(NEEDINIT) { }
					virtual ~EnableAddDefense() {}

					virtual void Initialize(IGameStateDatabase*);
					virtual void Destroy();
					virtual void Enable(bool enable);
				};

			}
		}
	}
}
