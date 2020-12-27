#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Defence : public State<Actor> {

	protected :
		float probability;
		uint32 type;

	public :
		Defence(Actor* owner, float prob, uint16 t)
			: State<Actor>(owner, Database::Info::StateInfo::Type::DEFENCE, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, probability(prob), type (t) {}
		virtual ~Defence() {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
