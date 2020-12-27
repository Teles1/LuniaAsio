#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class SelfHeal : public State<Actor> {
	protected :
		float amount;

	public :
		SelfHeal(Actor* owner, float a) : amount(a),
			State<Actor>(owner, Database::Info::StateInfo::Type::SELFHEAL, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}
		virtual ~SelfHeal() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
