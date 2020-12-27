#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class SpreadHate : public State<Actor> {
		const static float Tick;
		float time, hateRate, range;

	public :
		SpreadHate(Actor* owner, float hr, float r) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::SPREADHATE, Database::Info::StateInfo::Sort::ORDINARYSTATE), 
		   time(Tick), hateRate(hr), range(r) {}
		   virtual ~SpreadHate() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
