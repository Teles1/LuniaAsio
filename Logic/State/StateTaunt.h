#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Taunt : public State<Actor> {
	protected :
		float probability;
		float duration;
		float range;
	public :
		Taunt(Actor* owner, float prob, float r, float dur) : probability(prob), duration (dur), range(r),
			State<Actor>(owner, Database::Info::StateInfo::Type::TAUNT, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}
		virtual ~Taunt() {}

		void Initialize(IGameStateDatabase* db);
		bool Do(float, IGameStateDatabase*) { return true; }
	};

}	}	}	}
