#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Slow : public State<Actor> {
	protected :
		float rate;

	public :
		Slow(Actor* owner, float r) : rate(r),
			State<Actor>(owner, Database::Info::StateInfo::Type::SLOW, Database::Info::StateInfo::Sort::DEFENCESTATE) {}
		virtual ~Slow() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
