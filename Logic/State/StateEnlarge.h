#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class Enlarge : public State<Actor> {
		float rate;

	public :
		Enlarge(Actor* owner, float r) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::ENLARGE, Database::Info::StateInfo::Sort::DEFENCESTATE)
		   , rate(r) {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
