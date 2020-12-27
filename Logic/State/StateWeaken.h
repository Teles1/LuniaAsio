#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Weaken : public State<Actor> {
	protected:
		float lowerRate;

	public :
		Weaken(Actor* owner, float lower)
			: State<Actor>(owner, Database::Info::StateInfo::Type::WEAKEN, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, lowerRate(lower) {}
		virtual ~Weaken() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
