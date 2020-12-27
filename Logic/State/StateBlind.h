#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Blind : public State<Actor> {
	public :
		Blind(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::BLIND, Database::Info::StateInfo::Sort::DEFENCESTATE) {}

		void Initialize(IGameStateDatabase* db);
	};

}	}	}	}
