#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Sleep : public State<Actor> {
	protected :
		float range, duration, curse, buffRate;
	public :
		Sleep(Actor* owner, float r, float dur, float cur, float buff) : range(r), duration(dur), curse(cur), buffRate(buff),
		   State<Actor>(owner, Database::Info::StateInfo::Type::SLEEP, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}
	   virtual ~Sleep() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
