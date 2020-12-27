#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Hate : public State<Actor> {
	public :
		Hate(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::HATE, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}

		void Initialize(IGameStateDatabase* db);
		bool Do(float, IGameStateDatabase*) { return true; }
	};

}	}	}	}
