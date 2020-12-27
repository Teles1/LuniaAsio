#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Resurrection : public State<Actor> {
		float refillRate;

	public :
		Resurrection(Actor* owner, float h) :
			State<Actor>(owner, Database::Info::StateInfo::Type::RESURRECTION, Database::Info::StateInfo::Sort::ORDINARYSTATE),
			refillRate(h) {}
		virtual ~Resurrection() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
