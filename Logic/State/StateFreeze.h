#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Freeze : public State<Actor> {
	public :
		Freeze(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::FREEZE, Database::Info::StateInfo::Sort::DEFENCESTATE) {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
