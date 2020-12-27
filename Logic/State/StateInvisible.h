#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Invisible : public State<Actor> {
	public :
		Invisible(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::INVISIBLE, Database::Info::StateInfo::Sort::DEFENCESTATE) {}
	};

}	}	}	}
