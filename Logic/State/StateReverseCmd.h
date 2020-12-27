#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class ReverseCmd : public State<Actor> {
	public :
		ReverseCmd(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::REVERSECMD, Database::Info::StateInfo::Sort::DEFENCESTATE) {}

		void Initialize(IGameStateDatabase* db);
		void Destroy() {}
	};

}	}	}	}
