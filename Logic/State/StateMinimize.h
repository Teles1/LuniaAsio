#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Minimize : public State<Actor> {
	protected :
		float probability;

	public :
		Minimize(Actor* owner, float prob)
			: State<Actor>(owner, Database::Info::StateInfo::Type::MINIMIZE, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, probability(prob) {}
		virtual ~Minimize() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
